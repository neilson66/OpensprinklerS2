#include "bme280.h"

#if defined(ARDUINO) && defined(OS_DIY_GPIO)

#include <Arduino.h>
#include <Wire.h>

#include "../boards/diy_s2_config.h"

namespace {

struct Calib {
	uint16_t T1; int16_t T2, T3;
	uint16_t P1; int16_t P2, P3, P4, P5, P6, P7, P8, P9;
	uint8_t H1, H3; int16_t H2, H4, H5; int8_t H6;
};

Calib cal;
uint8_t addr = 0x76;
bool bus_started = false;
bool have_cal = false;
uint32_t last_ms = 0;
bool last_ok = false;
Bme280Reading last_reading = {0, 0, 0};

bool read_regs(uint8_t reg, uint8_t* buf, uint8_t n) {
	Wire.beginTransmission(addr);
	Wire.write(reg);
	if (Wire.endTransmission(false) != 0) return false;
	if (Wire.requestFrom((int)addr, (int)n) != n) return false;
	for (uint8_t i = 0; i < n; i++) buf[i] = Wire.read();
	return true;
}

bool write_reg(uint8_t reg, uint8_t val) {
	Wire.beginTransmission(addr);
	Wire.write(reg);
	Wire.write(val);
	return Wire.endTransmission() == 0;
}

bool find_chip() {
	static const uint8_t candidates[2] = {0x76, 0x77};
	for (uint8_t a : candidates) {
		addr = a;
		uint8_t id = 0;
		if (read_regs(0xD0, &id, 1) && id == 0x60) return true; // 0x60 = BME280 (0x58 would be a BMP280)
	}
	return false;
}

bool load_calibration() {
	uint8_t a[24], h1, e[7];
	if (!read_regs(0x88, a, 24) || !read_regs(0xA1, &h1, 1) || !read_regs(0xE1, e, 7)) return false;
	cal.T1 = a[0] | (a[1] << 8); cal.T2 = (int16_t)(a[2] | (a[3] << 8)); cal.T3 = (int16_t)(a[4] | (a[5] << 8));
	cal.P1 = a[6] | (a[7] << 8);
	cal.P2 = (int16_t)(a[8] | (a[9] << 8));   cal.P3 = (int16_t)(a[10] | (a[11] << 8));
	cal.P4 = (int16_t)(a[12] | (a[13] << 8)); cal.P5 = (int16_t)(a[14] | (a[15] << 8));
	cal.P6 = (int16_t)(a[16] | (a[17] << 8)); cal.P7 = (int16_t)(a[18] | (a[19] << 8));
	cal.P8 = (int16_t)(a[20] | (a[21] << 8)); cal.P9 = (int16_t)(a[22] | (a[23] << 8));
	cal.H1 = h1;
	cal.H2 = (int16_t)(e[0] | (e[1] << 8));
	cal.H3 = e[2];
	cal.H4 = (int16_t)(((int8_t)e[3] << 4) | (e[4] & 0x0F));
	cal.H5 = (int16_t)(((int8_t)e[5] << 4) | (e[4] >> 4));
	cal.H6 = (int8_t)e[6];
	return true;
}

} // namespace

bool bme280_read(Bme280Reading& out) {
	if (last_ms && (uint32_t)(millis() - last_ms) < 2000UL) {
		out = last_reading;
		return last_ok;
	}
	last_ms = millis();
	last_ok = false;

	if (!bus_started) {
		Wire.begin(osboard::DIY_I2C_SDA, osboard::DIY_I2C_SCL);
		Wire.setClock(100000);
		bus_started = true;
	}
	if (!have_cal) {
		if (!find_chip() || !load_calibration()) return false;
		have_cal = true;
	}

	// humidity x1, then temperature x1 / pressure x1 in forced mode (one-shot measurement)
	if (!write_reg(0xF2, 0x01) || !write_reg(0xF4, 0x25)) { have_cal = false; return false; }
	uint8_t status = 0x08;
	for (uint8_t i = 0; i < 20 && (status & 0x08); i++) {
		delay(2);
		if (!read_regs(0xF3, &status, 1)) { have_cal = false; return false; }
	}
	uint8_t d[8];
	if (!read_regs(0xF7, d, 8)) { have_cal = false; return false; }

	int32_t adc_P = ((int32_t)d[0] << 12) | ((int32_t)d[1] << 4) | (d[2] >> 4);
	int32_t adc_T = ((int32_t)d[3] << 12) | ((int32_t)d[4] << 4) | (d[5] >> 4);
	int32_t adc_H = ((int32_t)d[6] << 8) | d[7];

	// Bosch reference compensation formulas (BME280 datasheet, 32/64-bit integer versions)
	int32_t v1 = ((((adc_T >> 3) - ((int32_t)cal.T1 << 1))) * (int32_t)cal.T2) >> 11;
	int32_t v2 = (((((adc_T >> 4) - (int32_t)cal.T1) * ((adc_T >> 4) - (int32_t)cal.T1)) >> 12) * (int32_t)cal.T3) >> 14;
	int32_t t_fine = v1 + v2;
	float temp_c = ((t_fine * 5 + 128) >> 8) / 100.0f;

	int64_t p1 = (int64_t)t_fine - 128000;
	int64_t p2 = p1 * p1 * (int64_t)cal.P6;
	p2 += (p1 * (int64_t)cal.P5) << 17;
	p2 += ((int64_t)cal.P4) << 35;
	p1 = ((p1 * p1 * (int64_t)cal.P3) >> 8) + ((p1 * (int64_t)cal.P2) << 12);
	p1 = (((((int64_t)1) << 47) + p1)) * (int64_t)cal.P1 >> 33;
	if (p1 == 0) return false;
	int64_t p = 1048576 - adc_P;
	p = (((p << 31) - p2) * 3125) / p1;
	p1 = (((int64_t)cal.P9) * (p >> 13) * (p >> 13)) >> 25;
	p2 = (((int64_t)cal.P8) * p) >> 19;
	p = ((p + p1 + p2) >> 8) + (((int64_t)cal.P7) << 4);
	float press_kpa = (float)p / 256.0f / 1000.0f; // p is Pa in Q24.8

	int32_t x = t_fine - 76800;
	x = (((((adc_H << 14) - (((int32_t)cal.H4) << 20) - (((int32_t)cal.H5) * x)) + 16384) >> 15) *
		(((((((x * (int32_t)cal.H6) >> 10) * (((x * (int32_t)cal.H3) >> 11) + 32768)) >> 10) + 2097152) *
		(int32_t)cal.H2 + 8192) >> 14));
	x = x - (((((x >> 15) * (x >> 15)) >> 7) * (int32_t)cal.H1) >> 4);
	if (x < 0) x = 0;
	if (x > 419430400) x = 419430400;
	float hum = (x >> 12) / 1024.0f;

	last_reading = {temp_c, hum, press_kpa};
	last_ok = true;
	out = last_reading;
	return true;
}

#endif
