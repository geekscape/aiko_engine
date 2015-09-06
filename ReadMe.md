Aiko Engine
===========

Contents
--------
- [Overview](#overview)
- [ESP8266: Preparation](#preparation)
- [ESP8266: Flashing pre-built firmware](#flash)
- [ESP8266: Configuring Wi-Fi credentials](#configure)
- [Interactive session (LISP REPL)](#repl)
- [Resources](#resources)

Pages
-----
- [Software license](License)

<a name="overview" />
Overview
--------
The Aiko Engine provides ...
- Cross-platform framework and implementation for ESP8266 and Unix
- Abstractions for event handling, networking, persistence, timers
- Lisp scripting

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

<a name="repl" />
Interactive session (LISP REPL)
-------------------------------
Examples, particularly the
[aiko\_server](https://github.com/geekscape/aiko_engine/blob/master/examples/unix/aiko_server.c),
may include the
[embedded LISP engine](https://github.com/geekscape/aiko_engine/tree/master/src/common/lisp).

This provides an interactive Read-Eval-Print-Loop
(aka [REPL](https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop))
typically accessible via the serial console or a UDP socket.

The core LISP engine implements a minimal set of primitives:
car, cdr, cons, equal, atom, cond, lamdba, label, quote

Additional primitives have been provided ...

- (addTimer _PERIOD_ _COUNT_)
- (debug)
- (load)
- (save)
- (wifi (_SSID_ _PASSWORD_))

Currently, the REPL output only appears on the serial console.
Serial console input is not echoed (painful), so using UDP messages is
recommended ...

        nc -u ESP8266_IP_ADDRESS 4149
        (13:hello world !)

The REPL expects LISP input formatted as
[Canonical S-expressions](https://en.wikipedia.org/wiki/Canonical_S-expressions)

Examples ...

        6:anAtom                    ;; LISP atom (or token)
        (4:this2:is1:a4:list)       ;; Simple LISP list containing 4 tokens
        (8:addTimer)                ;; Fire a single timer after 1 second
        (8:addTimer4:2000)          ;; Fire a single timer after 2 seconds
        (8:addtimer4:2000:1:4)      ;; Fire a timer every 2 seconds, 4 times
        (5:debug)                   ;; Toggle debugging information
        (4:load)                    ;; Store configuration parameters
        (4:save)                    ;; Restore configuration parameters
        (4:wifi(4:ssid8:password))  ;; Set Wi-Fi Access Point credentials

<a name="resources" />
Resources
---------
Workshops ...
- [ESP8266 Workshops](https://github.com/geekscape/nodemcu_esp8266)

Associated open-source ESP8266 hardware ...
- [MeshThing](http://www.m9design.co) by m9design
- [John Spencer's](https://twitter.com/mage0r)
  [ESPkit-01 circuit board](https://github.com/mage0r/ESPkit-01)
- [Mark Wolfe's](https://twitter.com/wolfeidau)
  [Basic ESP board](https://github.com/wolfeidau/basic_esp_board)
