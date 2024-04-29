# pico-keypad

This project is a keypad/keyboard firmware for Raspberry Pi Pico using the pico SDK (C/C++).
I'm making this project without TinyUSB (it was on the first version) to understand USB protocol in depth. 

This project contain :
* a ws2812b (rgb led strip) driver using spi protocol
* a hw40 (rotary encoder) driver

## 📦 build

```sh
cd /Pico/pico-keypad/src/build
```

```sh
cmake -D PICO_KEYPAD_PATH=/Pico/pico-keypad ../../
```

```sh
make pico-keypad
```

```sh
cp /Pico/pico-keypad/src/build/pico-keypad.uf2 /media/user/RPI-RP2/
```

## 🗂️ ressources
* A Raspberry Pi Pico
* pico C/C++ SDK
* pi pico usb examples C/C++
* electronics stuff
* usb.org
* WireShark usb(very very useful)
* TinyUSB (as an example)

## todo
* transfer bigger than buffer size don't work without printf (bad func)
* windows & macos compatibility
* add led (Caps-lock...) read USB RX
