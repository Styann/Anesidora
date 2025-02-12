#ifndef USB_CONFIG_H
#define USB_CONFIG_H

#include "../lib/usb/usb.h"

const struct usb_device_descriptor device_descriptor = {
    .bLength            = sizeof(struct usb_device_descriptor),
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB             = 0x0110, // USB 1.1 device
    .bDeviceClass       = 0,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = 64,
    .idVendor           = 0x2E8A,
    .idProduct          = 0x0003,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = 0,
    .bNumConfigurations = 1
};

const struct usb_configuration_descriptor config_descriptor = {
    .bLength             = sizeof(struct usb_configuration_descriptor),
    .bDescriptorType     = USB_DESCRIPTOR_TYPE_CONFIG,
    .wTotalLength        = sizeof(struct usb_configuration_descriptor) + (
                                (sizeof(struct usb_interface_descriptor)
                                + sizeof(struct usb_hid_descriptor)
                                + sizeof(struct usb_endpoint_descriptor))
                            ),
    .bNumInterfaces      = 1,
    .bConfigurationValue = 1, // Configuration 1
    .iConfiguration      = 0, // No string
    .bmAttributes        = 0xA0, // self powered, remote wakeup
    .bMaxPower           = 50  // 100ma
};

const struct usb_interface_descriptor keyboard_interface_descriptor = {
    .bLength            = sizeof(struct usb_interface_descriptor),
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber   = 0,
    .bAlternateSetting  = 0,
    .bNumEndpoints      = 0x01, // Interface has 1 endpoints
    .bInterfaceClass    = USB_INTERFACE_CLASS_TYPE_HID,
    .bInterfaceSubClass = USB_INTERFACE_SUBCLASS_TYPE_BOOT_INTERFACE,
    .bInterfaceProtocol = USB_INTERFACE_PROTOCOL_KEYBOARD,
    .iInterface         = 0
};

const struct usb_endpoint_descriptor ep0_out_descriptor = {
    .bLength          = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = USB_DIR_OUT | 0, // EP number 0, OUT from host (rx to device)
    .bmAttributes     = USB_TRANSFER_TYPE_CONTROL,
    .wMaxPacketSize   = 64,
    .bInterval        = 0
};

const struct usb_endpoint_descriptor ep0_in_descriptor = {
    .bLength          = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = USB_DIR_IN | 0, // EP number 0, OUT from host (rx to device)
    .bmAttributes     = USB_TRANSFER_TYPE_CONTROL,
    .wMaxPacketSize   = 64,
    .bInterval        = 0
};

const struct usb_endpoint_descriptor ep1_in_descriptor = {
    .bLength          = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = USB_DIR_IN | 1,
    .bmAttributes     = USB_TRANSFER_TYPE_INTERRUPT,
    .wMaxPacketSize   = 64,
    .bInterval        = 5 // ms
};

const struct usb_endpoint_descriptor ep_control_descriptor = {
    .bLength = sizeof(struct usb_endpoint_descriptor),
    .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x40,
    .bmAttributes     = USB_TRANSFER_TYPE_CONTROL,
    .wMaxPacketSize   = 64,
    .bInterval        = 0
};

const struct usb_string_language_descriptor language_descriptor = {
    .bLength = sizeof(struct usb_string_language_descriptor),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wLANGID = USB_LANGUAGE_ID_FRENCH
};

const uint8_t keyboard_report_descriptor[] = {
    USAGE_PAGE, GENERIC_DESKTOP_PAGE,

    USAGE, USAGE_KEYBOARD,
    COLLECTION, COLLECTION_APPLICATION,
            REPORT_ID, 0x01,

            // keyboard
            USAGE_PAGE, KEYBOARD_KEYPAD_PAGE,
                // modifier field
                USAGE_MINIMUM, KC_CTRL_LEFT,
                USAGE_MAXIMUM, KC_GUI_RIGHT,
                LOGICAL_MINIMUM, 0x00,
                LOGICAL_MAXIMUM, 0x01,
                REPORT_SIZE, 0x01,
                REPORT_COUNT, 0x08,
                INPUT, DATA_VAR_ABS,
                // reserved field
                REPORT_COUNT, 0x01,
                REPORT_SIZE, 0x08,
                INPUT, CONST_ARRAY_ABS,
                REPORT_COUNT, 0x05,
                REPORT_SIZE, 0x01,
                // keycodes
                REPORT_COUNT, 0x06,
                REPORT_SIZE, 0x08,
                LOGICAL_MINIMUM, 0x00,
                LOGICAL_MAXIMUM, 0x65,
                USAGE_PAGE, KEYBOARD_KEYPAD_PAGE,
                USAGE_MINIMUM, 0x00,
                USAGE_MAXIMUM, 0xFF,
                INPUT, DATA_ARRAY_ABS,

            REPORT_ID, 0xC5,

            // led
            // USAGE_PAGE, LED_PAGE,
            //     USAGE_MINIMUM, 0x01,
            //     USAGE_MAXIMUM, 0x05,
            //     REPORT_SIZE, 0x01,
            //     REPORT_COUNT, 0x05,
            //     OUTPUT, DATA_VAR_ABS,
            //     REPORT_SIZE, 0x03,
            //     REPORT_COUNT, 0x01,
            //     OUTPUT, CONST_ARRAY_ABS,
    END_COLLECTION,

    USAGE_PAGE, CONSUMER_PAGE,
    USAGE, USAGE_CONSUMER_CONTROL,
    COLLECTION, COLLECTION_APPLICATION,
            REPORT_ID, 0x02,
            LOGICAL_MINIMUM, 0x00,
            LOGICAL_MAXIMUM + 1, 0xFF, 0x03,
            USAGE_MINIMUM, 0x00,
            0x2A, 0xFF, 0x03,
            REPORT_COUNT, 1,
            REPORT_SIZE, 16,
            INPUT, DATA_ARRAY_ABS,
    END_COLLECTION
};

const struct usb_hid_descriptor keyboard_hid_descriptor = {
    .bLength         = sizeof(struct usb_hid_descriptor),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_HID,
    .bcdHID          = 0x0111,
    .bCountryCode    = USB_COUNTRY_CODE_FRENCH,
    .bNumDescriptors = 0x01,
    .bReportType     = USB_DESCRIPTOR_TYPE_REPORT,
    .wReportLength   = sizeof(keyboard_report_descriptor)
};

#endif
