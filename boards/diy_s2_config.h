#pragma once
// Pin map and options for the DIY ESP32-S2 mini (LOLIN S2 mini) irrigation controller.
// Selected by building with -DOS_DIY_GPIO (see the os_diy_esp32s2 env in platformio.ini).
//
// Six relays are driven straight from GPIOs (no I2C expander, shift register, display,
// RTC, Ethernet or current sensing). Edit this file to match your wiring.

#include <stdint.h>

namespace osboard {

// Network name on the router and mDNS name (http://back-retic.local). Letters, digits and hyphens only.
constexpr const char* DIY_HOSTNAME = "back-retic";

constexpr uint8_t DIY_NUM_RELAYS = 6;

#if defined(RETIC_V2_PCB)
// "Neilsons Retic Controller V2.0" PCB (retic.kicad_pcb, 13 May 2021) with an S2 mini in the D1-mini socket.
// Each relay is driven GPIO -> 470R -> PC817 opto -> BC547 -> relay coil, so a HIGH output turns it ON.
// Station order follows the screw terminals: station 1 = M (master), 2..6 = zones 1..5.
//
//   station  terminal  relay  D1-mini pin  S2 mini GPIO
//      1        M        K6       D6           9
//      2        1        K5       D7          11
//      3        2        K4       D8          12
//      4        3        K3       D3          18
//      5        4        K2       D2          33
//      6        5        K1       D1          35
constexpr uint8_t DIY_RELAY_PINS[DIY_NUM_RELAYS] = {9, 11, 12, 18, 33, 35};
constexpr bool DIY_RELAY_ACTIVE_LOW = false;

// The relays use the S2 mini's SDA/SCL positions, so the BME280 goes on the extra INNER header row,
// next to those pins: GPIO34 (SDA) and GPIO36 (SCL). Address 0x76 or 0x77.
constexpr uint8_t DIY_I2C_SDA = 34;
constexpr uint8_t DIY_I2C_SCL = 36;

// The board's "1wire" terminal is D5 (GPIO7) with a 10k pull-up to 5V (R1): leave R1 unfitted before
// using it, since the S2 is not 5V tolerant. No other sensor inputs are wired.
constexpr uint8_t DIY_SENSOR1_PIN = 7;
constexpr uint8_t DIY_SENSOR2_PIN = 255;
#else
// Bench / generic wiring: six relay modules on GPIO 1..6.
// Avoid GPIO0, 45 and 46 (boot straps), 19/20 (USB), 26..32 (flash) and 15 (on-board LED).
constexpr uint8_t DIY_RELAY_PINS[DIY_NUM_RELAYS] = {1, 2, 3, 4, 5, 6};

// Most relay boards switch ON when the input is pulled LOW. Set to false for active-HIGH modules.
// Getting this wrong turns every valve ON at boot, so test with the water off.
constexpr bool DIY_RELAY_ACTIVE_LOW = true;

// I2C bus for the optional BME280 (temperature / humidity / pressure). These are the S2 mini's
// default SDA/SCL pins, in the same header positions as the D1 mini's D2/D1. Address 0x76 or 0x77.
constexpr uint8_t DIY_I2C_SDA = 33;
constexpr uint8_t DIY_I2C_SCL = 35;

// Optional rain / soil sensor inputs (dry contact to GND, internal pull-up). 255 = not fitted.
constexpr uint8_t DIY_SENSOR1_PIN = 7;
constexpr uint8_t DIY_SENSOR2_PIN = 8;
#endif

// Stations 7 and 8 exist in the UI (OpenSprinkler zones come in groups of 8) but have no output;
// disable them in the app (Edit Stations -> Disable).

} // namespace osboard
