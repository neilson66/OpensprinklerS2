#pragma once
// Minimal BME280 (temperature / humidity / pressure) reader for the DIY ESP32-S2 board.
#if defined(ARDUINO) && defined(OS_DIY_GPIO)

struct Bme280Reading {
	float temperature_c;
	float humidity_pct;
	float pressure_kpa;
};

// Takes one forced-mode measurement (cached for 2 s so several sensors can share it).
// Returns false if no BME280 answers at 0x76/0x77 or the read fails.
bool bme280_read(Bme280Reading& out);

#endif
