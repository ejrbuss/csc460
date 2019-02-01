#include "Peripherals.h"

//
// LASER
//

void init_laser() {
    pinMode(LASER_PIN, OUTPUT);
    digitalWrite(LASER_PIN, LOW);
}

//
// LCD
//

LiquidCrystal init_lcd() {
    LiquidCrystal lcd = LiquidCrystal(LCD_RS, LCD_EN, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);
    lcd.begin(16, 2);
    return lcd;
}

//
// STICK_U
//

int sample_stick_u_x() {
    static Q78_t rolling_x = Q78(0);
    Q78_t x = Q78((analogRead(STICK_U_PIN_X) - STICK_U_OFFSET_X) / STICK_SCALE);
    if (x < 0) {
        x = clamp(x + STICK_U_DEADZONE, STICK_U_MIN_X, 0);
    } else if (x > 0) {
        x = clamp(x - STICK_U_DEADZONE, 0, STICK_U_MAX_X);
    }
    rolling_x = Q78_lpf(x, rolling_x, STICK_LFP_FACTOR);
    return Q78_to_int(rolling_x);
}

int sample_stick_u_y() {
    static Q78_t rolling_y = Q78(0);
    Q78_t y = Q78((analogRead(STICK_U_PIN_Y) - STICK_U_OFFSET_Y) / STICK_SCALE);
    if (y < 0) {
        y = clamp(y + STICK_U_DEADZONE, STICK_U_MIN_Y, 0);
    } else if (y > 0) {
        y = clamp(y - STICK_U_DEADZONE, 0, STICK_U_MAX_Y);
    }
    rolling_y = Q78_lpf(y, rolling_y, STICK_LFP_FACTOR);
    return Q78_to_int(rolling_y);
}

void stick_u_on_switch(void (*isr)()) {
    pinMode(STICK_U_PIN_SW, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(STICK_U_PIN_SW), isr, CHANGE);
}

//
// STICK_M
//

int sample_stick_m_x() {
    static Q78_t rolling_x = Q78(0);
    Q78_t x = Q78((analogRead(STICK_M_PIN_X) - STICK_M_OFFSET_X) / STICK_SCALE);
    if (x < 0) {
        x = clamp(x + STICK_M_DEADZONE, STICK_M_MIN_X, 0);
    } else if (x > 0) {
        x = clamp(x - STICK_M_DEADZONE, 0, STICK_M_MAX_X);
    }
    rolling_x = Q78_lpf(x, rolling_x, STICK_LFP_FACTOR);
    return Q78_to_int(rolling_x);
}

int sample_stick_m_y() {
    static Q78_t rolling_y = Q78(0);
    Q78_t y = Q78((analogRead(STICK_M_PIN_Y) - STICK_M_OFFSET_Y) / STICK_SCALE);
    if (y < 0) {
        y = clamp(y + STICK_M_DEADZONE, STICK_M_MIN_Y, 0);
    } else if (y > 0) {
        y = clamp(y - STICK_M_DEADZONE, 0, STICK_M_MAX_Y);
    }
    rolling_y = Q78_lpf(y, rolling_y, STICK_LFP_FACTOR);
    return Q78_to_int(rolling_y);
}

void stick_m_on_switch(void (*isr)()) {
    pinMode(STICK_M_PIN_SW, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(STICK_M_PIN_SW), isr, CHANGE);
}

//
// PHOTOCELL
//

void init_photocell() {
    pinMode(PHOTO_PIN, INPUT);
}
