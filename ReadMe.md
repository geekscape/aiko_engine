Aiko Engine
===========

Contents
--------
- [Overview](#overview)
- [ESP8266: Preparation](#preparation)
- [ESP8266: Flashing pre-built firmware](#flash)
- [ESP8266: Configuring Wi-Fi credentials](#configure)
- [Software license](LICENSE)

<a name="overview" />
Overview
--------
[ESP8266 Workshops](https://github.com/geekscape/nodemcu_esp8266)

Recommended open-source ESP8266 hardware ...
- [John Spencer's](https://twitter.com/mage0r)
  [ESPkit-01 circuit board](https://github.com/mage0r/ESPkit-01)
- [Mark Wolfe's](https://twitter.com/wolfeidau)
  [Basic ESP board](https://github.com/wolfeidau/basic_esp_board)

<a name="preparation" />
ESP8266: Preparation
--------------------
- Acquire 3.3 VDC USB Serial adapter for flashing firmware / serial console
- git clone https://github.com/geekscape/aiko_engine.git
- cd aiko_engine
- git submodule update --init
- __Mac OS X or Linux:__ sudo pip install pyserial
- __Windows:__ Acquire
[nodemcu-flasher](https://github.com/nodemcu/nodemcu-flasher)

<a name="flash" />
ESP8266: Flashing pre-built firmware
------------------------------------
Milestone releases will be pre-built and placed in the _firmware/_ directory.
Either _esptool_ (Mac OS X or Linux) or _nodemcu\_flasher_ (Windows) can be
used to flash the firmware binaries to the ESP8266 SoC.

- Prepare ESP8266 SoC, place into bootloader mode ...

        Power off or hold [Reset] button
        Press and hold [Flash firmware] button (GPIO0 held low)
        Power on or release [Reset] button
        Release [Flash firmware] button

- Using _esptool_ in the top-level directory ...

        make SERIAL_PORT=/dev/tty.xxxx flash

- Successful firmware upgrade looks like ...

        Connecting...
        Erasing flash...
        Writing at 0x00008000... (100 %)
        Erasing flash...
        Writing at 0x0006bc00... (100 %)
        Leaving...

<a name="configure" />
ESP8266: Configuring Wi-Fi credentials
--------------------------------------
- In the top-level directory ...

        make aiko_configure

- Connect desktop computer to ESP8266 SoC Wi-Fi Soft Access Point
  - ESP8266 SoC Wi-Fi SSID will look like _ESP\_xxxxxx_

- Then type ...

        ./aiko_configure WIFI_SSID WIFI_PASSWORD

  - Where the parameters are for your Wi-Fi Access Point
  - __Caution: currently the Wi-Fi credentials are sent in plain-text (UNSECURE)__
