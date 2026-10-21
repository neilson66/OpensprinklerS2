#pragma once
// Stand-in for the SSD1306 OLED on boards that have no display (OS_DIY_GPIO).
// Every call is a no-op, so the UI/status code compiles unchanged.

#if defined(ARDUINO)

#include <Arduino.h>
#include "font.h"
#include "images.h"

#define LCD_STD 0
#define LCD_I2C 1
#ifndef BLACK
#define BLACK 0
#define WHITE 1
#endif
// SSD1306 text alignment values used by callers
#ifndef TEXT_ALIGN_LEFT
#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_RIGHT 1
#define TEXT_ALIGN_CENTER 2
#define TEXT_ALIGN_CENTER_BOTH 3
#endif

class SSD1306Display {
public:
	SSD1306Display(uint8_t, uint8_t, uint8_t) {}
	template <class... A> void init(A...) {}
	template <class... A> void begin(A...) {}
	template <class... A> void clear(A...) {}
	template <class... A> void display(A...) {}
	template <class... A> void displayOn(A...) {}
	template <class... A> void displayOff(A...) {}
	template <class... A> void setCursor(A...) {}
	template <class... A> void setAutoDisplay(A...) {}
	template <class... A> void setBrightness(A...) {}
	template <class... A> void setColor(A...) {}
	template <class... A> void setFont(A...) {}
	template <class... A> void setTextAlignment(A...) {}
	template <class... A> void createChar(A...) {}
	template <class... A> void drawString(A...) {}
	template <class... A> void drawXbm(A...) {}
	template <class... A> void drawProgressBar(A...) {}
	template <class... A> void fillCircle(A...) {}
	template <class... A> void noBlink(A...) {}
	template <class... A> void blink(A...) {}
	template <class... A> void backlight(A...) {}
	template <class... A> void noBacklight(A...) {}
	template <class T> size_t print(const T&) { return 0; }
	template <class T, class F> size_t print(const T&, F) { return 0; }
	template <class T> size_t println(const T&) { return 0; }
	size_t write(uint8_t) { return 0; }
	size_t write(const char*) { return 0; }
	uint8_t type() { return LCD_I2C; }
};

#endif
