#include "usb.h"
#include "../pico_extra/pico_extra.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/regs/usb.h" // USB register definitions from pico-sdk
#include "hardware/structs/usb.h" // USB hardware struct definitions from pico-sdk
#include "hardware/irq.h" // for interrupt enable and numbers
#include "hardware/resets.h" // for resetting the USB controller

#define usb_hw_set ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hw_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

/**
 * @brief set up the USB controller in device mode, clearing any previous state.
 */
void usb_device_init(usb_device_t *const device) {
    // reset usb controller
    reset_block(RESETS_RESET_USBCTRL_BITS);
    unreset_block_wait(RESETS_RESET_USBCTRL_BITS);

    // clear dpram
    memset(usb_dpram, 0, sizeof(*usb_dpram));

    // Mux the controller to the onboard usb phy
    usb_hw->muxing = USB_USB_MUXING_TO_PHY_BITS | USB_USB_MUXING_SOFTCON_BITS;

    // Force VBUS detect so the device thinks it is plugged into a host
    usb_hw->pwr = USB_USB_PWR_VBUS_DETECT_BITS | USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_BITS;

    // Enable the USB controller in device mode.
    usb_hw->main_ctrl = USB_MAIN_CTRL_CONTROLLER_EN_BITS;

    // Enable an interrupt per EP0 transaction
    usb_hw->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS;

    // Enable interrupts for when a buffer is done, when the bus is reset,
    // and when a setup packet is received
    usb_hw->inte = USB_INTS_BUFF_STATUS_BITS
        | USB_INTS_BUS_RESET_BITS
        | USB_INTS_SETUP_REQ_BITS
        | USB_INTS_DEV_SUSPEND_BITS
        | USB_INTS_DEV_RESUME_FROM_HOST_BITS;

    // Set up endpoints (endpoint control registers)
    // described by device configuration
    usb_setup_endpoints(device);

    // Present full speed device by enabling pull up on DP
    usb_hw_set->sie_ctrl = USB_SIE_CTRL_PULLUP_EN_BITS;

    // enable USB interrupt at processor
    irq_set_enabled(USBCTRL_IRQ, true);
}

/**
 * @brief used to get endpoint addr in memory
 * @param addr - usb endpoint addr
 */
struct usb_endpoint *usb_get_endpoint(usb_device_t *const device, const uint8_t addr) {
    for (uint i = 0; i < lengthof(device->endpoints); i++) {
        if (device->endpoints[i].descriptor && (device->endpoints[i].descriptor->bEndpointAddress == addr)) {
            return &device->endpoints[i];
        }
    }

    return NULL;
}

/**
 * @brief Take a buffer pointer located in the USB RAM and return as an offset of the RAM.
 * @param buf
 * @return uint32_t
 */
static inline uint32_t usb_buffer_offset(volatile uint8_t *buf) {
    return (uint32_t) buf ^ (uint32_t) usb_dpram;
}

/**
 * @brief Set up the endpoint control register for an endpoint (if applicable. Not valid for EP0).
 * @param ep
 */
void usb_setup_endpoint(struct usb_endpoint *ep) {
    // EP0 doesn't have one so return if that is the case
    if (!ep->endpoint_control) return;

    // Get the data buffer as an offset of the USB controller's DPRAM
    uint32_t dpram_offset = usb_buffer_offset(ep->data_buffer);
    uint32_t reg = EP_CTRL_ENABLE_BITS
                   | EP_CTRL_INTERRUPT_PER_BUFFER
                   | (ep->descriptor->bmAttributes << EP_CTRL_BUFFER_TYPE_LSB)
                   | dpram_offset;
    *ep->endpoint_control = reg;
}

/**
 * @brief Set up the endpoint control register for each endpoint.
 */
void usb_setup_endpoints(usb_device_t *const device) {
    for (uint i = 0; i < lengthof(device->endpoints); i++) {
        if (device->endpoints[i].descriptor && device->endpoints[i].handler) {
            usb_setup_endpoint(&(device->endpoints[i]));
        }
    }
}

/**
 * @brief Given an endpoint configuration, returns true if the endpoint
 * is transmitting data to the host (i.e. is an IN endpoint)
 * @param ep, the endpoint configuration
 * @return true
 * @return false
 */
static inline bool ep_is_tx(struct usb_endpoint *ep) {
    return ep->descriptor->bEndpointAddress & USB_DIR_IN;
}

void usb_xfer_pkt(struct usb_endpoint *ep, const uint8_t *buf, uint16_t len) {
    assert(len <= 64);

    // Prepare buffer control register value
    uint32_t val = len | USB_BUF_CTRL_AVAIL;

    if (ep_is_tx(ep)) {
        // Need to copy the data from the user buffer to the usb memory
        memcpy((void *)ep->data_buffer, (void *)buf, len);
        // Mark as full
        val |= USB_BUF_CTRL_FULL;
    }

    // Set pid and flip for next transfer
    val |= ep->next_pid ? USB_BUF_CTRL_DATA1_PID : USB_BUF_CTRL_DATA0_PID;
    ep->next_pid ^= 1;

    // val |= USB_BUF_CTRL_LAST;

    *ep->buffer_control = val & ~USB_BUF_CTRL_AVAIL;

    busy_wait_at_least_cycles(12);

    *ep->buffer_control = val;
}

/**
 * @param ep
 * @param buf
 * @param len
 */
void usb_xfer(struct usb_endpoint *ep, const uint8_t *buf, const uint16_t len) {
    if (len == 0) {
        usb_xfer_pkt(ep, buf, len);
    }

    uint16_t remaining_len = len;
    size_t ep_buf_size = ep->descriptor->wMaxPacketSize;

    while (remaining_len > 0) {
        size_t pkt_size = MIN(remaining_len, ep_buf_size);

        usb_xfer_pkt(ep, buf, pkt_size);

        while (*ep->buffer_control & USB_BUF_CTRL_FULL) {
            tight_loop_contents();
        }

        buf += pkt_size;
        remaining_len -= pkt_size;
    }
}

void usb_control_xfer(usb_device_t *const device, uint8_t *buf, uint16_t len) {
    usb_xfer(usb_get_endpoint(device, USB_DIR_OUT), buf, len);
}

void usb_xfer_ep0_in(usb_device_t *const device, const uint8_t *buf, const uint16_t len) {
    usb_xfer(usb_get_endpoint(device, USB_DIR_IN), buf, len);
}

/**
 * @brief Handle a BUS RESET from the host by setting the device address back to 0.
 */
void usb_bus_reset(usb_device_t *const device) {
    // Set address back to 0
    device->addr = 0;
    device->should_set_addr = false;
    usb_hw->dev_addr_ctrl = 0;
    device->configured = false;
}

/**
 * @brief Sends a zero length status packet back to the host.
 */
void usb_acknowledge_out_request(usb_device_t *const device) {
    usb_xfer(usb_get_endpoint(device, USB_DIR_IN), NULL, 0);
}

/**
 * @brief Handles a SET_ADDR request from the host. The actual setting of the device address in
 * hardware is done in ep0_in_handler. This is because we have to acknowledge the request first
 * as a device with address zero.
 * @param pkt, the setup packet from the host.
 */
void usb_set_device_address(usb_device_t *const device, const uint8_t addr) {
    // Set address is a bit of a strange case because we have to send a 0 length status packet first with
    // address 0
    device->addr = addr;
    // Will set address in the callback phase
    device->should_set_addr = true;
    usb_acknowledge_out_request(device);
}

/**
 * @brief Handles a SET_CONFIGRUATION request from the host. Assumes one configuration so simply
 * sends a zero length status packet back to the host.
 * @param pkt, the setup packet from the host.
 */
void usb_set_device_configuration(usb_device_t *const device) {
    // Only one configuration so just acknowledge the request
    usb_acknowledge_out_request(device);
    device->configured = true;
}

/**
 * @brief Respond to a setup packet from the host.
 */
void usb_handle_setup_packet(usb_device_t *const device, volatile struct usb_setup_packet *pkt) {
    // Reset PID to 1 for EP0 IN
    usb_get_endpoint(device, USB_DIR_IN)->next_pid = 1u;

    switch (pkt->bmRequestType) {
        case USB_DIR_OUT: {
            switch (pkt->bRequest) {
                case USB_REQUEST_SET_ADDRESS:
                    usb_set_device_address(device, pkt->wValue & 0xFF);
                    break;
                case USB_REQUEST_SET_CONFIGURATION:
                    usb_set_device_configuration(device);
                    break;
                default:
                    usb_acknowledge_out_request(device);
            }

            break;
        }
        case USB_DIR_IN: {
            if (pkt->bRequest == USB_REQUEST_GET_DESCRIPTOR) {
                const uint16_t descriptor_type = pkt->wValue >> 8;

                switch (descriptor_type) {
                    case USB_DESCRIPTOR_TYPE_DEVICE:
                        usb_handle_device_descriptor(device, pkt->wLength);
                        break;
                    case USB_DESCRIPTOR_TYPE_CONFIG:
                        usb_handle_config_descriptor(device, pkt->wLength);
                        break;
                    case USB_DESCRIPTOR_TYPE_STRING:
                        usb_handle_string_descriptor(device, pkt->wValue & 0xFF, pkt->wIndex, pkt->wLength);
                        break;
                    default:
                        usb_acknowledge_out_request(device);
                }
            }

            break;
        }
        case 0x21: {
            switch (pkt->bRequest) {
                case USB_REQUEST_SET_IDLE:
                    usb_acknowledge_out_request(device);
                    break;
                case USB_REQUEST_SET_REPORT:
                    set_report_callback(usb_get_endpoint(device, 0x81u)->data_buffer, pkt->wLength);
                    break;
                default:
                    usb_acknowledge_out_request(device);
            }

            break;
        }
        case EP_IN_HID: {
            switch (pkt->bRequest) {
                case USB_REQUEST_GET_DESCRIPTOR: {
                    const uint16_t descriptor_type = pkt->wValue >> 8;

                    if (descriptor_type == USB_DESCRIPTOR_TYPE_REPORT) {
                        usb_handle_hid_report(U16_LOW(pkt->wValue), pkt->wIndex, pkt->wLength);
                    }
                }
                default:
                    usb_acknowledge_out_request(device);
            }

            break;
        }
        default:
            usb_acknowledge_out_request(device);
    }
}

void usb_suspend_callback(void) {

}

void usb_handle_suspend(usb_device_t *const device) {
    if (device->configured && !device->suspended) {
        device->suspended = true;
        usb_suspend_callback();
    }
}

void usb_resume_callback(void) {

}

void usb_handle_resume(usb_device_t *const device) {
    if (device->configured && device->suspended) {
        device->suspended = false;
        usb_resume_callback();
    }
}

void usb_remote_wakeup(usb_device_t *const device) {
    if (device->configured && device->suspended) {
        device->suspended = false;
        usb_resume_callback();
    }
}

void isr_usbctrl_task(usb_device_t *const device) {
    uint32_t status = usb_hw->ints;
    uint32_t handled = 0;

    // Setup packet received
    if (status & USB_INTS_SETUP_REQ_BITS) {
        handled |= USB_INTS_SETUP_REQ_BITS;
        volatile struct usb_setup_packet *pkt = (volatile struct usb_setup_packet *)&usb_dpram->setup_packet;
        usb_handle_setup_packet(device, pkt);
        usb_hw_clear->sie_status = USB_SIE_STATUS_SETUP_REC_BITS;
    }

    // Buffer status, one or more buffers have completed
    if (status & USB_INTS_BUFF_STATUS_BITS) {
        usb_handle_buff_status(device);
        handled |= USB_INTS_BUFF_STATUS_BITS;
    }

    // Bus is reset
    if (status & USB_INTS_BUS_RESET_BITS) {
        handled |= USB_INTS_BUS_RESET_BITS;
        usb_bus_reset(device);
        usb_hw_clear->sie_status = USB_SIE_STATUS_BUS_RESET_BITS;
    }

    // Device is suspended
    if (status & USB_INTS_DEV_SUSPEND_BITS) {
        handled |= USB_INTS_DEV_SUSPEND_BITS;
        usb_handle_suspend(device);
        usb_hw_clear->sie_status = USB_SIE_STATUS_SUSPENDED_BITS;
    }

    // Device is resumed
    if (status & USB_INTS_DEV_RESUME_FROM_HOST_BITS) {
        handled |= USB_INTS_DEV_RESUME_FROM_HOST_BITS;
        usb_handle_resume(device);
        usb_hw_clear->sie_status = USB_SIE_STATUS_RESUME_BITS;
    }

    if (status ^ handled) {
        panic("Unhandled IRQ 0x%x\n", (uint) (status ^ handled));
    }
}

void set_report_callback(volatile uint8_t *buf, uint16_t len) {

}

/**
 * @brief send device descriptor to host
 * @param wLength
 */
void usb_handle_device_descriptor(usb_device_t *const device, const uint16_t wLength) {
    // Always respond with pid 1
    usb_get_endpoint(device, USB_DIR_IN)->next_pid = 1;
    usb_xfer_ep0_in(device, (uint8_t *)device->device_descriptor, wLength);
}


/**
 * @param descriptorIndex
 * @param languageId
 */
void usb_handle_string_descriptor(usb_device_t *const device, const uint8_t descriptorIndex, const uint16_t languageId, const uint16_t wLength) {
    if (descriptorIndex == 0) {
        usb_xfer_ep0_in(device, (uint8_t *)device->language_descriptor, sizeof(device->language_descriptor));
    }
    else {
        char const *const *string = (descriptorIndex == 1) ? &device->vendor : &device->product;
        const uint string_len = strlen(*string);

        uint8_t bLength = 2 + string_len * 2;
        if (bLength % 2) bLength++;

        uint16_t string_descriptor_buffer[127];
        memset(string_descriptor_buffer, 0, sizeof(string_descriptor_buffer));

        string_descriptor_buffer[0] = USB_DESCRIPTOR_TYPE_STRING << 8 | bLength;
        utf8_to_utf16(*string, string_len, &string_descriptor_buffer[1]);

        usb_xfer_ep0_in(device, (uint8_t *)string_descriptor_buffer, bLength);
    }
}

/**
 * @brief Notify an endpoint that a transfer has completed.
 * @param ep, the endpoint to notify.
 */
static void usb_handle_ep_buff_done(struct usb_endpoint *ep) {
    uint32_t buffer_control = *ep->buffer_control;
    // Get the transfer length for this endpoint
    uint16_t len = buffer_control & USB_BUF_CTRL_LEN_MASK;

    // Call that endpoints buffer done handler
    ep->handler((uint8_t*)ep->data_buffer, len);
}

/**
 * @brief Find the endpoint configuration for a specified endpoint number and
 * direction and notify it that a transfer has completed.
 * @param ep_num
 * @param in
 */
static void usb_handle_buff_done(usb_device_t *const device, uint ep_num, bool in) {
    const uint8_t ep_addr = (in ? USB_DIR_IN : USB_DIR_OUT) | ep_num ;

    for (uint i = 0; i < lengthof(device->endpoints); i++) {
        struct usb_endpoint *const ep = &(device->endpoints[i]);

        if (ep->descriptor && ep->handler) {
            if (ep->descriptor->bEndpointAddress == ep_addr) {
                usb_handle_ep_buff_done(ep);
                return;
            }
        }
    }
}

/**
 * @brief Handle a "buffer status" irq. This means that one or more
 * buffers have been sent / received. Notify each endpoint where this
 * is the case.
 */
static void usb_handle_buff_status(usb_device_t *const device) {
    uint32_t buffers = usb_hw->buf_status;
    uint32_t remaining_buffers = buffers;

    uint bit = 1u;
    for (uint i = 0; remaining_buffers && i < 3 * 2; i++) {
        if (remaining_buffers & bit) {
            // clear this in advance
            usb_hw_clear->buf_status = bit;
            // IN transfer for even i, OUT transfer for odd i
            usb_handle_buff_done(device, i >> 1u, !(i & 1u));
            remaining_buffers &= ~bit;
        }
        bit <<= 1u;
    }
}
