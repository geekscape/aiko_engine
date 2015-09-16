Aiko Engine
===========

Contents
--------
- [Overview](#overview)
- [Unix: Build examples](#unix_build)
- [Interactive session (LISP REPL)](#lisp_repl)
- [ESP8266: Preparation](#esp8266_preparation)
- [ESP8266: Flashing pre-built firmware](#esp8266_flash)
- [ESP8266: Configuring Wi-Fi credentials](#esp8266_configure)
- [ESP8266: Build firmware on Linux](#esp8266_build)
- [Resources](#resources)

Pages
-----
- [Contributors](Contributors.md)
- [Software license](License)

<a name="overview" />
Overview
--------
The Aiko Engine provides ...

- Cross-platform framework and implementation for ESP8266 and Unix
- Abstractions for event handling, networking, persistence, timers
- Lisp scripting

<a name="unix_build" />
Unix: Build examples
--------------------
To build the various [Unix examples](examples/unix),
in the top-level directory on either Linux or Mac OS X, type ...

        make

<a name="lisp_repl" />
Interactive session (LISP REPL)
-------------------------------
Examples, particularly the
[aiko\_server](https://github.com/geekscape/aiko_engine/blob/master/examples/unix/aiko_server.c),
may include the
[embedded LISP engine](https://github.com/geekscape/aiko_engine/tree/master/src/common/lisp).

This provides an interactive Read-Eval-Print-Loop
(aka [REPL](https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop))
typically accessible via the serial console or over a UDP socket.

The core LISP engine implements a
[minimal set of primitives](http://ep.yimg.com/ty/cdn/paulgraham/jmc.lisp):
car, cdr, cons, equal, atom, cond, lamdba, label, quote

Additional primitives have been provided ...

        (addTimer PERIOD COUNT)
        (debug)
        (load)
        (save)
        (wifi (SSID PASSWORD))

Currently, the REPL output only appears on the serial console.
Serial console input is not echoed (awkward to use),
so using UDP messages is recommended ...

        nc -u ESP8266_IP_ADDRESS 4149
        (13:hello world !)

The REPL expects LISP input formatted as
[Canonical S-expressions](https://en.wikipedia.org/wiki/Canonical_S-expressions)

Examples ...

        6:anAtom                    ;; LISP atom (or symbol)
        (4:this2:is1:a4:list)       ;; Simple LISP list containing 4 tokens
        (8:addTimer)                ;; Fire a single timer after 1 second
        (8:addTimer4:2000)          ;; Fire a single timer after 2 seconds
        (8:addTimer4:20001:4)       ;; Fire a timer every 2 seconds, 4 times
        (5:debug)                   ;; Toggle debugging information
        (4:load)                    ;; Store configuration parameters
        (4:save)                    ;; Restore configuration parameters
        (4:wifi(4:ssid8:password))  ;; Set Wi-Fi Access Point credentials

<a name="esp8266_preparation" />
ESP8266: Preparation
--------------------
- Acquire USB Serial adapter for flashing firmware / serial console
  - Note: Voltage needs to match that required for your ESP8266 hardware

        git clone https://github.com/geekscape/aiko_engine.git
        cd aiko_engine
        git submodule update --init

- __Mac OS X or Linux:__ sudo pip install pyserial
- __Windows:__ Acquire
[nodemcu-flasher](https://github.com/nodemcu/nodemcu-flasher)

<a name="esp8266_flash" />
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

<a name="esp8266_configure" />
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

<a name="esp8266_build" />
Build: ESP8266 firmware on Linux
--------------------------------
To build the various [ESP8266 examples](examples/esp8266),
the ESP8266 cross-compiler development environment must installed.
Presently, that is only supported on Linux.  If you use Mac OS X or
Windows, then your best option is to install Ubuntu on a Virtual Machine.

Note: Currently, the Aiko Engine works on ESP8266 SDK 1.2.0, but not 1.3.0.

- Install Ubuntu VM
- Install ansible on your local host
  - brew install ansible ssh-copy-id
  - ssh-copy-id USERNAME@HOSTNAME\_VM
- git clone https://github.com/wolfeidau/ansible-esp8266-role
- cd ansible-esp8266-role
- vi inventory\_HOSTNAME\_VM

        [HOSTNAME_VM]
        HOSTNAME_VM.local ansible_ssh_user=USERNAME ask_pass=True

- cp playbook.yml playbook\_HOSTNAME\_VM.yml
- vi playbook\_HOSTNAME\_VM.yml

        ---
        - name: Test the ESP8266 role
          hosts: all
          sudo: yes
          tasks:
            - include: "tasks/main.yml" 
              vars:
                deploy_user: USERNAME 
                esp_iot_sdk_archive_url: "http://bbs.espressif.com/download/file.php?id=564&sid=b012c7cc3f156b4a934c60f8275afc7a"
                esp_iot_sdk: "esp_iot_sdk_v1.2.0_15_07_03.zip"
                esp_iot_sdk_path: "esp_iot_sdk_v1.2.0"

- ansible-playbook -i inventory\_HOSTNAME\_VM.yml playbook.HOSTNAME\_VM.yml --ask-sudo-pass --check

If the _ansible-playbook_ dry-run works, then you can do it for real ...

- ansible-playbook -i inventory\_HOSTNAME\_VM.yml playbook.HOSTNAME\_VM.yml --ask-sudo-pass

Now that the ESP8266 cross-compiler development environment is installed,
you can login to your Ubuntu VM and update your executable PATH ...

- vi /etc/profile.d/esp8266.sh 

        export PATH=/opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin:$PATH

Building and flashing the ESP8266 firmware on your Ubuntu VM ...

        git clone https://github.com/geekscape/aiko_engine.git
        cd aiko_engine
        git submodule update --init
        cd examples/esp8266/aiko_server
        make
        make flash

The final step assumes that you have your USB-serial adapter connected as
_/dev/ttyUSB0_ and you've pressed the [Reset] and [Flash] buttons correctly.

Further details on the [ESP8266 firmware flash procedure](#esp8266_flash)
and [ESP8266 Wi-Fi configuration](esp8266_configure) are above.

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
