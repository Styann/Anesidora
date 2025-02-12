#ifndef USB_STRUCTS
#define USB_STRUCTS

#include "pico/types.h"

struct usb_setup_packet {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __packed;

enum bmRequestType {
    USB_REQUEST_TYPE_STANDARD = 0x00u,
    USB_REQUEST_TYPE_TYPE_MASK = 0x60u,
    USB_REQUEST_TYPE_TYPE_CLASS = 0x20u,
    USB_REQUEST_TYPE_TYPE_VENDOR = 0x40u,
    USB_REQUEST_TYPE_RECIPIENT_MASK = 0x1fu,
    USB_REQUEST_TYPE_RECIPIENT_DEVICE = 0x00u,
    USB_REQUEST_TYPE_RECIPIENT_INTERFACE = 0x01u,
    USB_REQUEST_TYPE_RECIPIENT_ENDPOINT = 0x02u
};

enum bRequest {
    USB_REQUEST_GET_STATUS        = 0x00,
    USB_REQUEST_CLEAR_FEATURE     = 0x01,
    USB_REQUEST_SET_FEATURE       = 0x03,
    USB_REQUEST_SET_ADDRESS       = 0x05,
    USB_REQUEST_GET_DESCRIPTOR    = 0x06,
    USB_REQUEST_SET_DESCRIPTOR    = 0x07,
    USB_REQUEST_GET_CONFIGURATION = 0x08,
    USB_REQUEST_SET_CONFIGURATION = 0x09,
    USB_REQUEST_GET_INTERFACE     = 0x0a,
    USB_REQUEST_SET_INTERFACE     = 0x0b,
    USB_REQUEST_SYNC_FRAME        = 0x0c,
    USB_REQUEST_GET_IDLE          = 0x02,
    USB_REQUEST_SET_IDLE          = 0x0a,
    USB_REQUEST_SET_REPORT        = 0x09
};

struct usb_endpoint {
        const struct usb_endpoint_descriptor *descriptor;
        void (*handler)(uint8_t *buf, uint16_t len);

        // Pointers to endpoint + buffer control registers
        // in the USB controller DPSRAM
        volatile uint32_t *endpoint_control;
        volatile uint32_t *buffer_control;
        volatile uint8_t *data_buffer;

        // Toggle after each packet (unless replying to a SETUP)
        uint8_t next_pid;
};

struct usb_hid_interface {
    const struct usb_interface_descriptor *const interface_descriptor;
    const struct usb_hid_descriptor *const hid_descriptor;
    struct usb_endpoint *const endpoint;
};

typedef struct {
    uint8_t addr;
    bool should_set_addr;
    volatile bool configured;
    volatile bool suspended;

    struct usb_endpoint endpoints[3];

    const struct usb_device_descriptor *const device_descriptor;
    const struct usb_configuration_descriptor *const configuration_descriptor;
    const struct usb_string_language_descriptor *const language_descriptor;
    const char *const vendor;
    const char *const product;
} usb_device_t;

#endif
