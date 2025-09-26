#ifndef USB_HID_H
#define USB_HID_H

#include "pico/stdlib.h"
#include "usb_structs.h"

#define EP_IN_HID 0x81

// this shit is wrong
struct usb_gamepad_report {
    // uint8_t id;
    uint8_t x;
    uint8_t y;
    uint8_t buttons;
} __packed;

struct usb_mouse_report {
    uint8_t id;
    uint8_t buttons;
    int16_t  x;
    int16_t  y;
    int8_t  wheel;
    // int8_t  pan;
} __packed;

struct usb_keyboard_report {
    uint8_t id;
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keycodes[6];
} __packed;

struct usb_consumer_control_report {
    uint8_t id;
    uint16_t consumer_control;
} __packed;

void usb_send_keyboard_report(struct usb_endpoint *const endpoint, const struct usb_keyboard_report *const report);

void usb_send_mouse_report(struct usb_endpoint *const endpoint, const struct usb_mouse_report *const report);

void usb_send_single_keycode(struct usb_endpoint *const endpoint, uint8_t keycode);

void release_keyboard(struct usb_endpoint *const endpoint);

void usb_send_consumer_control_report(struct usb_endpoint *const endpoint, const struct usb_consumer_control_report *const report);

void usb_send_consumer_control(struct usb_endpoint *const endpoicodcnt, const uint16_t consumer_control);

void usb_send_gamepad_report(struct usb_endpoint *const endpoint, const struct usb_gamepad_report *const report);

uint hid_interface_cpy(uint8_t *buffer, const struct usb_hid_interface *const hid_interface);

enum usb_keycode {
    USB_KEY_NONE                       = 0x00,
    USB_KEY_ERROR_ROLL_OVER            = 0x01,
    USB_KEY_POSTFAIL                   = 0x02,
    USB_KEY_ERROR_UNDEFINED            = 0x03,
    USB_KEY_A                          = 0x04,
    USB_KEY_B                          = 0x05,
    USB_KEY_C                          = 0x06,
    USB_KEY_D                          = 0x07,
    USB_KEY_E                          = 0x08,
    USB_KEY_F                          = 0x09,
    USB_KEY_G                          = 0x0A,
    USB_KEY_H                          = 0x0B,
    USB_KEY_I                          = 0x0C,
    USB_KEY_J                          = 0x0D,
    USB_KEY_K                          = 0x0E,
    USB_KEY_L                          = 0x0F,
    USB_KEY_M                          = 0x10,
    USB_KEY_N                          = 0x11,
    USB_KEY_O                          = 0x12,
    USB_KEY_P                          = 0x13,
    USB_KEY_Q                          = 0x14,
    USB_KEY_R                          = 0x15,
    USB_KEY_S                          = 0x16,
    USB_KEY_T                          = 0x17,
    USB_KEY_U                          = 0x18,
    USB_KEY_V                          = 0x19,
    USB_KEY_W                          = 0x1A,
    USB_KEY_X                          = 0x1B,
    USB_KEY_Y                          = 0x1C,
    USB_KEY_Z                          = 0x1D,
    USB_KEY_1                          = 0x1E,
    USB_KEY_2                          = 0x1F,
    USB_KEY_3                          = 0x20,
    USB_KEY_4                          = 0x21,
    USB_KEY_5                          = 0x22,
    USB_KEY_6                          = 0x23,
    USB_KEY_7                          = 0x24,
    USB_KEY_8                          = 0x25,
    USB_KEY_9                          = 0x26,
    USB_KEY_0                          = 0x27,
    USB_KEY_ENTER                      = 0x28,
    USB_KEY_ESCAPE                     = 0x29,
    USB_KEY_BACKSPACE                  = 0x2A,
    USB_KEY_TAB                        = 0x2B,
    USB_KEY_SPACE                      = 0x2C,
    USB_KEY_MINUS                      = 0x2D, // -
    USB_KEY_EQUAL                      = 0x2E, // =
    USB_KEY_SQUARE_BRACKET_LEFT        = 0x2F, // { [
    USB_KEY_SQUARE_BRACKET_RIGHT       = 0x30, // } ]
    USB_KEY_BACKSLASH                  = 0x31,
    USB_KEY_EUROPE_1                   = 0x32,
    USB_KEY_SEMICOLON                  = 0x33, // ; :
    USB_KEY_APOSTROPHE                 = 0x34,
    USB_KEY_GRAVE                      = 0x35, // ` ~
    USB_KEY_COMMA                      = 0x36, // , <
    USB_KEY_PERIOD                     = 0x37, // . >
    USB_KEY_SLASH                      = 0x38,
    USB_KEY_CAPS_LOCK                  = 0x39,
    USB_KEY_F1                         = 0x3A,
    USB_KEY_F2                         = 0x3B,
    USB_KEY_F3                         = 0x3C,
    USB_KEY_F4                         = 0x3D,
    USB_KEY_F5                         = 0x3E,
    USB_KEY_F6                         = 0x3F,
    USB_KEY_F7                         = 0x40,
    USB_KEY_F8                         = 0x41,
    USB_KEY_F9                         = 0x42,
    USB_KEY_F10                        = 0x43,
    USB_KEY_F11                        = 0x44,
    USB_KEY_F12                        = 0x45,
    USB_KEY_PRINT_SCREEN               = 0x46,
    USB_KEY_SCROLL_LOCK                = 0x47,
    USB_KEY_PAUSE                      = 0x48,
    USB_KEY_INSERT                     = 0x49,
    USB_KEY_HOME                       = 0x4A,
    USB_KEY_PAGE_UP                    = 0x4B,
    USB_KEY_DELETE                     = 0x4C,
    USB_KEY_END                        = 0x4D,
    USB_KEY_PAGE_DOWN                  = 0x4E,
    USB_KEY_ARROW_RIGHT                = 0x4F,
    USB_KEY_ARROW_LEFT                 = 0x50,
    USB_KEY_ARROW_DOWN                 = 0x51,
    USB_KEY_ARROW_UP                   = 0x52,
    USB_KEY_NUM_LOCK                   = 0x53,
    USB_KEY_KEYPAD_DIVIDE              = 0x54,
    USB_KEY_KEYPAD_MULTIPLY            = 0x55,
    USB_KEY_KEYPAD_SUBTRACT            = 0x56,
    USB_KEY_KEYPAD_ADD                 = 0x57,
    USB_KEY_KEYPAD_ENTER               = 0x58,
    USB_KEY_KEYPAD_1                   = 0x59,
    USB_KEY_KEYPAD_2                   = 0x5A,
    USB_KEY_KEYPAD_3                   = 0x5B,
    USB_KEY_KEYPAD_4                   = 0x5C,
    USB_KEY_KEYPAD_5                   = 0x5D,
    USB_KEY_KEYPAD_6                   = 0x5E,
    USB_KEY_KEYPAD_7                   = 0x5F,
    USB_KEY_KEYPAD_8                   = 0x60,
    USB_KEY_KEYPAD_9                   = 0x61,
    USB_KEY_KEYPAD_0                   = 0x62,
    USB_KEY_KEYPAD_PERIOD              = 0x63,
    USB_KEY_EUROPE_2                   = 0x64,
    USB_KEY_APPLICATION                = 0x65,
    USB_KEY_POWER                      = 0x66,
    USB_KEY_KEYPAD_EQUAL               = 0x67,
    USB_KEY_F13                        = 0x68,
    USB_KEY_F14                        = 0x69,
    USB_KEY_F15                        = 0x6A,
    USB_KEY_F16                        = 0x6B,
    USB_KEY_F17                        = 0x6C,
    USB_KEY_F18                        = 0x6D,
    USB_KEY_F19                        = 0x6E,
    USB_KEY_F20                        = 0x6F,
    USB_KEY_F21                        = 0x70,
    USB_KEY_F22                        = 0x71,
    USB_KEY_F23                        = 0x72,
    USB_KEY_F24                        = 0x73,
    USB_KEY_EXECUTE                    = 0x74,
    USB_KEY_HELP                       = 0x75,
    USB_KEY_MENU                       = 0x76,
    USB_KEY_SELECT                     = 0x77,
    USB_KEY_STOP                       = 0x78,
    USB_KEY_AGAIN                      = 0x79,
    USB_KEY_UNDO                       = 0x7A,
    USB_KEY_CUT                        = 0x7B,
    USB_KEY_COPY                       = 0x7C,
    USB_KEY_PASTE                      = 0x7D,
    USB_KEY_FIND                       = 0x7E,
    USB_KEY_MUTE                       = 0x7F,
    USB_KEY_VOLUME_UP                  = 0x80,
    USB_KEY_VOLUME_DOWN                = 0x81,
    USB_KEY_LOCKING_CAPS_LOCK          = 0x82,
    USB_KEY_LOCKING_NUM_LOCK           = 0x83,
    USB_KEY_LOCKING_SCROLL_LOCK        = 0x84,
    USB_KEY_KEYPAD_COMMA               = 0x85,
    USB_KEY_KEYPAD_EQUAL_SIGN          = 0x86,
    USB_KEY_KANJI1                     = 0x87,
    USB_KEY_KANJI2                     = 0x88,
    USB_KEY_KANJI3                     = 0x89,
    USB_KEY_KANJI4                     = 0x8A,
    USB_KEY_KANJI5                     = 0x8B,
    USB_KEY_KANJI6                     = 0x8C,
    USB_KEY_KANJI7                     = 0x8D,
    USB_KEY_KANJI8                     = 0x8E,
    USB_KEY_KANJI9                     = 0x8F,
    USB_KEY_LANG1                      = 0x90,
    USB_KEY_LANG2                      = 0x91,
    USB_KEY_LANG3                      = 0x92,
    USB_KEY_LANG4                      = 0x93,
    USB_KEY_LANG5                      = 0x94,
    USB_KEY_LANG6                      = 0x95,
    USB_KEY_LANG7                      = 0x96,
    USB_KEY_LANG8                      = 0x97,
    USB_KEY_LANG9                      = 0x98,
    USB_KEY_ALTERNATE_ERASE            = 0x99,
    USB_KEY_SYSREQ_ATTENTION           = 0x9A,
    USB_KEY_CANCEL                     = 0x9B,
    USB_KEY_CLEAR                      = 0x9C,
    USB_KEY_PRIOR                      = 0x9D,
    USB_KEY_RETURN                     = 0x9E,
    USB_KEY_SEPARATOR                  = 0x9F,
    USB_KEY_OUT                        = 0xA0,
    USB_KEY_OPER                       = 0xA1,
    USB_KEY_CLEAR_AGAIN                = 0xA2,
    USB_KEY_CRSEL_PROPS                = 0xA3,
    USB_KEY_EXSEL                      = 0xA4,
    // RESERVED					    0xA5 to 0xAF
    USB_KEY_KEYPAD_00                  = 0xB0,
    USB_KEY_KEYPAD_000                 = 0xB1,
    USB_KEY_THOUSAND_SEPARATOR         = 0xB2,
    USB_KEY_DECIMAL_SEPARATOR          = 0xB3,
    USB_KEY_CURRENCY_UNIT              = 0xB4,
    USB_KEY_CURRENCY_SUBUNIT           = 0xB5,
    USB_KEY_KEYPAD_BRACKET_LEFT        = 0xB6,
    USB_KEY_KEYPAD_BRACKET_RIGHT       = 0xB7,
    USB_KEY_KEYPAD_CURLY_BRACKET_LEFT  = 0xB8,
    USB_KEY_KEYPAD_CURLY_BRACKET_RIGHT = 0xB9,
    USB_KEY_KEYPAD_TAB                 = 0xBA,
    USB_KEY_KEYPAD_BACKSPACE           = 0xBB,
    USB_KEY_KEYPAD_A                   = 0xBC,
    USB_KEY_KEYPAD_B                   = 0xBD,
    USB_KEY_KEYPAD_C                   = 0xBE,
    USB_KEY_KEYPAD_D                   = 0xBF,
    USB_KEY_KEYPAD_E                   = 0xC0,
    USB_KEY_KEYPAD_F                   = 0xC1,
    USB_KEY_KEYPAD_XOR                 = 0xC2,
    USB_KEY_KEYPAD_CIRCUMFLEX          = 0xC3, // ^
    USB_KEY_KEYPAD_PERCENT             = 0xC4,
    USB_KEY_KEYPAD_CHEVRON_LEFT        = 0xC5, // <
    USB_KEY_KEYPAD_CHEVRON_RIGHT       = 0xC6, // >
    USB_KEY_KEYPAD_AMPERSAND           = 0xC7, // &
    USB_KEY_KEYPAD_AND                 = 0xC8, // &&
    USB_KEY_KEYPAD_PIPE                = 0xC9, // |
    USB_KEY_KEYPAD_OR                  = 0xCA, // ||
    USB_KEY_KEYPAD_COLON               = 0xCB, // :
    USB_KEY_KEYPAD_HASHTAG             = 0xCC,
    USB_KEY_KEYPAD_SPACE               = 0xCD,
    USB_KEY_KEYPAD_AT                  = 0xCE, // @
    USB_KEY_KEYPAD_EXCLAMATION_MARK    = 0xCF, // !
    USB_KEY_KEYPAD_MEMORY_STORE        = 0xD0,
    USB_KEY_KEYPAD_MEMORY_RECALL       = 0xD1,
    USB_KEY_KEYPAD_MEMORY_CLEAR        = 0xD2,
    USB_KEY_KEYPAD_MEMORY_ADD          = 0xD3,
    USB_KEY_KEYPAD_MEMORY_SUBTRACT     = 0XD4,
    USB_KEY_KEYPAD_MEMORY_MULTIPLY     = 0xD5,
    USB_KEY_KEYPAD_MEMORY_DIVIDE       = 0xD6,
    USB_KEY_KEYPAD_PLUS_OR_MINUS       = 0xD7,
    USB_KEY_KEYPAD_CLEAR               = 0xD8,
    USB_KEY_KEYPAD_CLEAR_ENTRY         = 0xD9,
    USB_KEY_KEYPAD_BINARY              = 0xDA,
    USB_KEY_KEYPAD_OCTAL               = 0xDB,
    USB_KEY_KEYPAD_DECIMAL             = 0xDC,
    USB_KEY_KEYPAD_HEXADECIMAL         = 0xDD,
    // RESERVED                     0xDE to 0xDF
    USB_KEY_CTRL_LEFT                  = 0xE0,
    USB_KEY_SHIFT_LEFT                 = 0xE1,
    USB_KEY_ALT_LEFT                   = 0xE2,
    USB_KEY_GUI_LEFT                   = 0xE3,
    USB_KEY_CTRL_RIGHT                 = 0xE4,
    USB_KEY_SHIFT_RIGHT                = 0xE5,
    USB_KEY_ALT_RIGHT                  = 0xE6,
    USB_KEY_GUI_RIGHT                  = 0xE7
    // RESERVED                     0xE8 to 0xFFFF
};

enum usb_key_modifier {
    USB_KEY_MODIFIER_NONE        = 0b00000000,
    USB_KEY_MODIFIER_CTRL_LEFT   = 0b00000001,
    USB_KEY_MODIFIER_SHIFT_LEFT  = 0b00000010,
    USB_KEY_MODIFIER_ALT_LEFT    = 0b00000100,
    USB_KEY_MODIFIER_GUI_LEFT    = 0b00001000,
    USB_KEY_MODIFIER_CTRL_RIGHT  = 0b00010000,
    USB_KEY_MODIFIER_SHIFT_RIGHT = 0b00100000,
    USB_KEY_MODIFIER_ALT_RIGHT   = 0b01000000,
    USB_KEY_MODIFIER_GUI_RIGHT   = 0b10000000
};

/**
 *  OOC -> On/Off Control
 *  OSC -> One Shot Control
 *  LC  -> Linear Control
 *  RTC -> Re-Trigger Control
 */
enum usb_consumer_control {
    USB_CONSUMER_CONTROL_BRIGHTNESS_INCREMENT = 0x006F,
    USB_CONSUMER_CONTROL_BRIGHTNESS_DECREMENT = 0x0070,
    USB_CONSUMER_CONTROL_NEXT_TRACK           = 0x00B5,
    USB_CONSUMER_CONTROL_PREVIOUS_TRACK       = 0x00B6,
    USB_CONSUMER_CONTROL_RANDOM_PLAY          = 0x00B9,
    USB_CONSUMER_CONTROL_PLAY_PAUSE           = 0x00CD,
    USB_CONSUMER_CONTROL_MUTE                 = 0x00E2,
    USB_CONSUMER_CONTROL_VOLUME_INCREMENT     = 0x00E9,
    USB_CONSUMER_CONTROL_VOLUME_DECREMENT     = 0x00EA,
    USB_CONSUMER_CONTROL_RECORD               = 0x00B2
};

enum usb_led {
    USB_LED_UNDEFINED                  = 0x00,
    USB_LED_NUM_LOCK                   = 0x01,
    USB_LED_CAPS_LOCK                  = 0x02,
    USB_LED_SCROLL_LOCK                = 0x03,
    USB_LED_COMPOSE                    = 0x04,
    USB_LED_KANA                       = 0x05,
    USB_LED_POWER                      = 0x06,
    USB_LED_SHIFT                      = 0x07,
    USB_LED_DO_NOT_DISTURB             = 0x08,
    USB_LED_MUTE                       = 0x09,
    USB_LED_TONE_ENABLE                = 0x0A,
    USB_LED_HIGH_CUT_FILTER            = 0x0B,
    USB_LED_LOW_CUT_FILTER             = 0x0C,
    USB_LED_EQUALIZER_ENABLE           = 0x0D,
    USB_LED_SOUND_FIELD                = 0x0E,
    USB_LED_SURROUND_ON                = 0x0F,
    USB_LED_REPEAT                     = 0x10,
    USB_LED_STEREO                     = 0x11,
    USB_LED_SAMPLING_RATE_DETECT       = 0x12,
    USB_LED_SPINNING                   = 0x13,
    USB_LED_CAV                        = 0x14,
    USB_LED_CLV                        = 0x15,
    USB_LED_RECORDING_FORMAT_DETECT    = 0x16,
    USB_LED_OFF_HOOK                   = 0x17,
    USB_LED_RING                       = 0x18,
    USB_LED_MESSAGE_WAITING            = 0x19,
    USB_LED_DATA_MODE                  = 0x1A,
    USB_LED_BATTERY_OPERATION          = 0x1B,
    USB_LED_BATTERY_OK                 = 0x1C,
    USB_LED_BATTERY_LOW                = 0x1D,
    USB_LED_SPEAKER                    = 0x1E,
    USB_LED_HEADSET                    = 0x1F,
    USB_LED_HOLD                       = 0x20,
    USB_LED_MICROPHONE                 = 0x21,
    USB_LED_COVERAGE                   = 0x22,
    USB_LED_NIGHT_MODE                 = 0x23,
    USB_LED_SEND_CALLS                 = 0x24,
    USB_LED_CALL_PICKUP                = 0x25,
    USB_LED_CONFERENCE                 = 0x26,
    USB_LED_STAND_BY                   = 0x27,
    USB_LED_CAMERA_ON                  = 0x28,
    USB_LED_CAMERA_OFF                 = 0x29,
    USB_LED_ON_LINE                    = 0x2A,
    USB_LED_OFF_LINE                   = 0x2B,
    USB_LED_BUSY                       = 0x2C,
    USB_LED_READY                      = 0x2D,
    USB_LED_PAPER_OUT                  = 0x2E,
    USB_LED_PAPER_JAM                  = 0x2F,
    USB_LED_REMOTE                     = 0x30,
    USB_LED_FORWARD                    = 0x31,
    USB_LED_REVERSE                    = 0x32,
    USB_LED_STOP                       = 0x33,
    USB_LED_REWIND                     = 0x34,
    USB_LED_FAST_FORWARD               = 0x35,
    USB_LED_PLAY                       = 0x36,
    USB_LED_PAUSE                      = 0x37,
    USB_LED_RECORD                     = 0x38,
    USB_LED_ERROR                      = 0x39,
    USB_LED_USAGE_SELECTED_INDICATOR   = 0x3A,
    USB_LED_USAGE_IN_USE_INDICATOR     = 0x3B,
    USB_LED_USAGE_MULTI_MODE_INDICATOR = 0x3C,
    USB_LED_INDICATOR_ON               = 0x3D,
    USB_LED_INDICATOR_FLASH            = 0x3E,
    USB_LED_INDICATOR_SLOW_BLINK       = 0x3F,
    USB_LED_INDICATOR_FAST_BLINK       = 0x40,
    USB_LED_INDICATOR_OFF              = 0x41,
    USB_LED_FLASH_ON_TIME              = 0x42,
    USB_LED_SLOW_BLINK_ON_TIME         = 0x43,
    USB_LED_SLOW_BLINK_OFF_TIME        = 0x44,
    USB_LED_FAST_BLINK_ON_TIME         = 0x45,
    USB_LED_FAST_BLINK_OFF_TIME        = 0x46,
    USB_LED_USAGE_INDICATOR_COLOR      = 0x47,
    USB_LED_INDICATOR_RED              = 0x48,
    USB_LED_INDICATOR_GREEN            = 0x49,
    USB_LED_INDICATOR_AMBER            = 0x4A,
    USB_LED_GENERIC_INDICATOR          = 0x4B,
    USB_LED_SYSTEM_SUSPEND             = 0x4C,
    USB_LED_EXTERNAL_POWER_CONNECTED   = 0x4D,
    USB_LED_INDICATOR_BLUE             = 0x4E,
    USB_LED_INDICATOR_ORANGE           = 0x4F,
    USB_LED_GOOD_STATUS                = 0x50,
    USB_LED_WARNING_STATUS             = 0x51,
    USB_LED_RGB_LED                    = 0x52,
    USB_LED_RED_LED_CHANNEL            = 0x53,
    USB_LED_BLUE_LED_CHANNEL           = 0x54,
    USB_LED_GREEN_LED_CHANNEL          = 0x55,
    USB_LED_LED_INTENSITY              = 0x56,
    USB_LED_SYSTEM_MICROPHONE_MUTE     = 0x57,
    // RESERVED                        0x5F to 0x58
    USB_LED_PLAYER_INDICATOR           = 0x60,
    USB_LED_PLAYER_1                   = 0x61,
    USB_LED_PLAYER_2                   = 0x62,
    USB_LED_PLAYER_3                   = 0x63,
    USB_LED_PLAYER_4                   = 0x64,
    USB_LED_PLAYER_5                   = 0x65,
    USB_LED_PLAYER_6                   = 0x66,
    USB_LED_PLAYER_7                   = 0x67,
    USB_LED_PLAYER_8                   = 0x68
    // RESERVED                        0x69 to 0xFFF
};

enum usb_gamepad_button {
    USB_GAMEPAD_BUTTON_A      = 1 << 0,
    USB_GAMEPAD_BUTTON_B      = 1 << 1,
    USB_GAMEPAD_BUTTON_C      = 1 << 2,
    USB_GAMEPAD_BUTTON_X      = 1 << 3,
    USB_GAMEPAD_BUTTON_Y      = 1 << 4,
    USB_GAMEPAD_BUTTON_Z      = 1 << 5,
    USB_GAMEPAD_BUTTON_LT     = 1 << 6,
    USB_GAMEPAD_BUTTON_RT     = 1 << 7,
    USB_GAMEPAD_BUTTON_LB     = 1 << 8,
    USB_GAMEPAD_BUTTON_RB     = 1 << 9,
    USB_GAMEPAD_BUTTON_SELECT = 1 << 10,
    USB_GAMEPAD_BUTTON_START  = 1 << 11
};

#endif
