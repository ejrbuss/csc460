#include "Peripherals.h"

#define MAX_LASER_TIME (10 * 1000) // ten seconds
#define MAX_HIT_TIME   (2 * 1000)  // two seconds

using namespace RTOS;

//
// LASER
//

void init_laser() {
    pinMode(LASER_PIN, OUTPUT);
    digitalWrite(LASER_PIN, LOW);
}

void set_laser(int on) {
    static i64 laser_time = 0;
    static i64 last       = 0;
    static bool last_on   = false;

    if (laser_time > MAX_LASER_TIME) {
        digitalWrite(LASER_PIN, OFF);
        return;
    }
    if (on) {
        i64 now = Time::now();
        if (last_on) {
            laser_time += now - last;
        }
        last = now;
    }
    digitalWrite(LASER_PIN, on);
    last_on = on;
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
    static Q78_t sample_x  = Q78(0);
    sample_x  = Q78((analogRead(STICK_U_PIN_X) - STICK_U_OFFSET_X) / STICK_SCALE);
    rolling_x = Q78_lpf(sample_x, rolling_x, STICK_LFP_FACTOR);
    int x     = Q78_to_int(rolling_x);
    if (x < 0) {
        x = clamp(x + STICK_U_DEADZONE, STICK_U_MIN_X, 0);
    } else if (x > 0) {
        x = clamp(x - STICK_U_DEADZONE, 0, STICK_U_MAX_X);
    }
    return x;
}

int sample_stick_u_y() {
    static Q78_t rolling_y = Q78(0);
    static Q78_t sample_y  = Q78(0);
    sample_y  = Q78((analogRead(STICK_U_PIN_Y) - STICK_U_OFFSET_Y) / STICK_SCALE);
    rolling_y = Q78_lpf(sample_y, rolling_y, STICK_LFP_FACTOR);
    int y     = Q78_to_int(rolling_y);
    if (y < 0) {
        y = clamp(y + STICK_U_DEADZONE, STICK_U_MIN_Y, 0);
    } else if (y > 0) {
        y = clamp(y - STICK_U_DEADZONE, 0, STICK_U_MAX_Y);
    }
    return y;
}

void stick_u_on_switch(void (*isr)()) {
    attachInterrupt(digitalPinToInterrupt(STICK_U_PIN_SW), isr, CHANGE);
}

void init_stick_u_sw() {
    pinMode(STICK_U_PIN_SW, INPUT_PULLUP);
}

int stick_u_down() {
    return !digitalRead(STICK_U_PIN_SW);
}

//
// STICK_M
//

int sample_stick_m_x() {
    static Q78_t rolling_x = Q78(0);
    static Q78_t sample_x  = Q78(0);
    sample_x  = Q78((analogRead(STICK_M_PIN_X) - STICK_M_OFFSET_X) / STICK_SCALE);
    rolling_x = Q78_lpf(sample_x, rolling_x, STICK_LFP_FACTOR);
    int x     = Q78_to_int(rolling_x);
    if (x < 0) {
        x = clamp(x + STICK_M_DEADZONE, STICK_M_MIN_X, 0);
    } else if (x > 0) {
        x = clamp(x - STICK_M_DEADZONE, 0, STICK_M_MAX_X);
    }
    return x;
}

int sample_stick_m_y() {
    static Q78_t rolling_y = Q78(0);
    static Q78_t sample_y  = Q78(0);
    sample_y  = Q78((analogRead(STICK_M_PIN_Y) - STICK_M_OFFSET_Y) / STICK_SCALE);
    rolling_y = Q78_lpf(sample_y, rolling_y, STICK_LFP_FACTOR);
    int y     = Q78_to_int(rolling_y);
    if (y < 0) {
        y = clamp(y + STICK_M_DEADZONE, STICK_M_MIN_Y, 0);
    } else if (y > 0) {
        y = clamp(y - STICK_M_DEADZONE, 0, STICK_M_MAX_Y);
    }
    return y;
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

bool photocell_hit() {
    static i64 hit_time = 0;
    static i64 last     = 0;
    static bool last_on = false;

    bool on = analogRead(PHOTO_PIN) >= PHOTO_HIT_THRESHOLD;

    if (hit_time > MAX_HIT_TIME) {
        return true;
    }
    if (on) {
        i64 now = Time::now();
        if (last_on) {
            hit_time += now - last;
        }
        last = now;
    }
    last_on = on;
    return false;
}

//
// SERVO_PAN
//

static Servo servo_pan;

void init_servo_pan() {
    servo_pan.attach(SERVO_PAN_PIN);
}

void map_servo_pan(int value, int min_value, int max_value) {
    static int servo_pan_position = SERVO_PAN_CENTER;
    static int last_call = 0;
    int this_call = Time::now();
    if (this_call - last_call < SERVO_PAN_DELAY) {
        return;
    }
    last_call = this_call;
    Q78_t range = Q78(SERVO_PAN_MAX_SPEED);
    Q78_t ratio = Q78_div(Q78(value - min_value), Q78(max_value));
    Q78_t delta = Q78_mul(range, ratio);
    servo_pan_position += clamp(Q78_to_int(delta), -SERVO_PAN_MAX_SPEED, SERVO_PAN_MAX_SPEED);
    servo_pan_position =  clamp(servo_pan_position, SERVO_PAN_BOTTOM, SERVO_PAN_TOP);
    // RTOS::debug_print("pan:  %d\n", servo_pan_position);
    servo_pan.writeMicroseconds(servo_pan_position);
}

//
// SERVO_TILT
//

static Servo servo_tilt;

void init_servo_tilt() {
    servo_tilt.attach(SERVO_TILT_PIN);
}

void map_servo_tilt(int value, int min_value, int max_value) {
    static int servo_tilt_position = SERVO_TILT_CENTER;
    static int last_call = 0;
    int this_call = RTOS::Time::now();
    if (this_call - last_call < SERVO_TILT_DELAY) {
        return;
    }
    last_call = this_call;
    Q78_t range = Q78(SERVO_TILT_MAX_SPEED);
    Q78_t ratio = Q78_div(Q78(value - min_value), Q78(max_value));
    Q78_t delta = Q78_mul(range, ratio);
    servo_tilt_position += clamp(Q78_to_int(delta), -SERVO_TILT_MAX_SPEED, SERVO_TILT_MAX_SPEED);
    servo_tilt_position =  clamp(servo_tilt_position, SERVO_TILT_BOTTOM, SERVO_TILT_TOP);
    // RTOS::debug_print("tilt: %d\n", servo_tilt_position);
    servo_tilt.writeMicroseconds(servo_tilt_position);
}
