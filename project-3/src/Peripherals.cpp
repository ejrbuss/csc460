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
// PHOTOCELL
//

void init_photocell() {
    pinMode(PHOTO_PIN, INPUT);
}

bool photocell_hit() {
    static i64 hit_time = 0;
    static i64 last     = 0;
    static bool last_on = false;
    static int rolling_avg = 0;
    
    int sample = analogRead(PHOTO_PIN);
    rolling_avg = sample * 0.2 + rolling_avg * 0.8;
    // debug_print("sample: %d, \tavg: %d\n", sample, rolling_avg);
    
    bool on = rolling_avg >= PHOTO_HIT_THRESHOLD;

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
