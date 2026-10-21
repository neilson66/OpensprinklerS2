#pragma once
// Pin map and options for the DIY ESP32-S2 mini (LOLIN S2 mini) irrigation controller.
// Selected by building with -DOS_DIY_GPIO (see the os_diy_esp32s2 env in platformio.ini).
//
// Six relays are driven straight from GPIOs (no I2C expander, shift register, display,
// RTC, Ethernet or current sensing). Edit this file to match your wiring.

#include <stdint.h>

namespace osboard {

constexpr uint8_t DIY_NUM_RELAYS = 6;

// Relay GPIOs, station 1..6. Stations 7 and 8 exist in the UI (OpenSprinkler zones come in
// groups of 8) but have no output; disable them in the app (Edit Stations -> Disable).
// Avoid GPIO0, 45 and 46 (boot straps), 19/20 (USB), 26..32 (flash) and 15 (on-board LED).
constexpr uint8_t DIY_RELAY_PINS[DIY_NUM_RELAYS] = {1, 2, 3, 4, 5, 6};

// Most relay boards switch ON when the input is pulled LOW. Set to false for active-HIGH modules.
// Getting this wrong turns every valve ON at boot, so test with the water off.
constexpr bool DIY_RELAY_ACTIVE_LOW = true;

// Optional rain / soil sensor inputs (dry contact to GND, internal pull-up). 255 = not fitted.
constexpr uint8_t DIY_SENSOR1_PIN = 7;
constexpr uint8_t DIY_SENSOR2_PIN = 8;

} // namespace osboard
