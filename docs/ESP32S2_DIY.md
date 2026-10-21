# DIY ESP32-S2 mini build (6 relays on GPIO)

Build: `pio run -e os_diy_esp32s2` (run from PowerShell/cmd, not Git Bash). Flash: `pio run -e os_diy_esp32s2 -t upload`.
First flash: hold BOOT, tap RST on the S2 mini to enter the bootloader.

Everything you may need to change is in `boards/diy_s2_config.h`:

| Item | Default |
|---|---|
| Relays, stations 1-6 | GPIO 1, 2, 3, 4, 5, 6 |
| Relay polarity | active LOW (`DIY_RELAY_ACTIVE_LOW`) |
| Rain/soil sensor 1, 2 | GPIO 7, 8 (to GND, internal pull-up) |

Intended use: a *remote* station box. The main OpenSprinkler adds these as "Remote Station (IP)" stations and owns the programs and master logic, so no master is set here. Relay 1 = station 1 (the back master solenoid).

Not present on this board: I2C expanders/expansion boards, OLED, buttons, RTC (time comes from NTP),
Ethernet, current sensing, RF, latching/DC boost. Stations 7-8 have no output; disable them in the app.
Firmware update from the app is not wired up (signed OS4 images); update over USB or ArduinoOTA-style flashing.
