cmake_minimum_required(VERSION 3.25)

include(pico_sdk_import.cmake)

project(anesidora C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

message("ANESIDORA_PATH is $ENV{ANESIDORA_PATH}")

add_executable(anesidora
    $ENV{ANESIDORA_PATH}/src/anesidora.c
    $ENV{ANESIDORA_PATH}/lib/pico_extra/pico_extra.c
    $ENV{ANESIDORA_PATH}/lib/debounce/debounce.c
    $ENV{ANESIDORA_PATH}/lib/keyboard/keyboard.c
    $ENV{ANESIDORA_PATH}/lib/usb/usb.c
    $ENV{ANESIDORA_PATH}/lib/usb/usb_hid.c
    $ENV{ANESIDORA_PATH}/lib/rotary_encoder/rotary_encoder.c
    $ENV{ANESIDORA_PATH}/lib/ws2812b/ws2812b.c
    $ENV{ANESIDORA_PATH}/lib/ssd1331/ssd1331.c
    $ENV{ANESIDORA_PATH}/lib/ssd1306/ssd1306.c
    $ENV{ANESIDORA_PATH}/lib/st7735s/st7735s.c
    $ENV{ANESIDORA_PATH}/lib/button/button.c
    $ENV{ANESIDORA_PATH}/lib/joystick8way/joystick8way.c
    $ENV{ANESIDORA_PATH}/lib/graphics/color.c
    $ENV{ANESIDORA_PATH}/lib/led/led.c
)

target_link_libraries(anesidora
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_resets
    hardware_irq
    hardware_pwm
    hardware_spi
    hardware_i2c
)

pico_add_extra_outputs(anesidora)

message(${CMAKE_C_COMPILER})
message(${CMAKE_CXX_COMPILER})
message(${CMAKE_CXX_COMPILER_AR})
message(${CMAKE_CXX_COMPILER_RANLIB})
