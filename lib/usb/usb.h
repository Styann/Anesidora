#ifndef USB_H
#define USB_H

#include <string.h>

#include "usb_structs.h"
#include "usb_descriptors.h"
#include "usb_hid.h"

#define USB_DIR_OUT 0x00u
#define USB_DIR_IN 0x80u

void usb_device_init(usb_device_t *const device);

struct usb_endpoint *usb_get_endpoint(usb_device_t *const device, const uint8_t addr);

void set_report_callback(volatile uint8_t *buf, uint16_t len) __attribute__((weak));

void usb_remote_wakeup(usb_device_t *const device);

static inline uint32_t usb_buffer_offset(volatile uint8_t *buf);

void usb_setup_endpoint(struct usb_endpoint *ep);

void usb_setup_endpoints(usb_device_t *const device);

static inline bool ep_is_tx(struct usb_endpoint *ep);

void usb_xfer_pkt(struct usb_endpoint *ep, const uint8_t *buf, uint16_t len);

void usb_xfer(struct usb_endpoint *ep, const uint8_t *buf, const uint16_t len);

void usb_control_xfer(usb_device_t *const device, uint8_t *buf, uint16_t len);

void usb_xfer_ep0_in(usb_device_t *const device, const uint8_t *buf, const uint16_t len);

void usb_acknowledge_out_request(usb_device_t *const device);

// all next functions are used into isr_usbctrl()
void isr_usbctrl_task(usb_device_t *const device);

    void usb_handle_setup_packet(usb_device_t *const device, volatile struct usb_setup_packet *pkt);

        void usb_set_device_address(usb_device_t *const device, const uint8_t addr);

        void usb_set_device_configuration(usb_device_t *const device);

        void usb_handle_device_descriptor(usb_device_t *const device, const uint16_t wLength);

        void usb_handle_config_descriptor(usb_device_t *const device, const uint16_t wLength) __attribute__((weak));

        void usb_handle_string_descriptor(usb_device_t *const device, const uint8_t descriptorIndex, const uint16_t languageId, const uint16_t wLength);

        extern void usb_handle_hid_report(const uint8_t bDescriptorIndex, const uint16_t wInterfaceNumber, const uint16_t wDescriptorLength);

    static void usb_handle_buff_status(usb_device_t *const device);

    void usb_bus_reset(usb_device_t *const device);

    void usb_handle_suspend(usb_device_t *const device);

        void usb_suspend_callback(void) __attribute__((weak));

    void usb_handle_resume(usb_device_t *const device);

        void usb_resume_callback(void) __attribute__((weak));

static void usb_handle_ep_buff_done(struct usb_endpoint *ep);

static void usb_handle_buff_done(usb_device_t *const device, uint ep_num, bool in);

#endif
