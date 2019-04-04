#include "Stick.h"

#define LPF_FACTOR 100 // As a percent between 1 and 100

namespace Stick {

    i16 clamp(i16 value, i16 min_value, i16 max_value) {
        i16 tmp = value < min_value ? min_value : value;
        return tmp > max_value ? max_value : tmp;
    }

    Stick_t * init_stick_u() {
        Stick_t * stick_u = (Stick_t *) RTOS::Memory::static_alloc("stick_u", sizeof(Stick_t));
        stick_u->pin_x     = 10;
        stick_u->pin_y     = 11;
        stick_u->pin_sw    = 51;
        stick_u->offset_x  = 506;
        stick_u->offset_y  = 506;
        stick_u->deadzone  = 100;
        stick_u->max_x     = stick_u->offset_x - stick_u->deadzone;
        stick_u->max_y     = stick_u->offset_y - stick_u->deadzone;
        stick_u->rolling_x = 0;
        stick_u->rolling_y = 0;
        pinMode(stick_u->pin_sw, INPUT_PULLUP);
        return stick_u;
    }

    Stick_t * init_stick_m() {
        Stick_t * stick_m = (Stick_t *) RTOS::Memory::static_alloc("stick_m", sizeof(Stick_t));
        stick_m->pin_x     = 8;
        stick_m->pin_y     = 9;
        stick_m->pin_sw    = 50;
        stick_m->offset_x  = 506;
        stick_m->offset_y  = 506;
        stick_m->deadzone  = 100;
        stick_m->max_x     = stick_m->offset_x - stick_m->deadzone;
        stick_m->max_y     = stick_m->offset_y - stick_m->deadzone;
        stick_m->rolling_x = 0;
        stick_m->rolling_y = 0;
        pinMode(stick_m->pin_sw, INPUT_PULLUP);
        return stick_m;
    }

    i8 x(Stick_t * stick) {
        i16 sample = analogRead(stick->pin_x) - stick->offset_x;
        i16 fx = (sample * LPF_FACTOR + stick->rolling_x * (100 - LPF_FACTOR)) / 100;
        stick->rolling_x = fx;
        if (fx > 0) {
            fx = clamp(fx - stick->deadzone, 0, stick->max_x);
        } else if (fx < 0) {
            fx = clamp(fx + stick->deadzone, -stick->max_x, 0);
        }
        f32 ratio = ((f32) fx) / stick->max_x;
        return ratio * 127;
    }

    i8 y(Stick_t * stick) {
        i16 sample = analogRead(stick->pin_y) - stick->offset_y;
        i16 fy = (sample * LPF_FACTOR + stick->rolling_y * (100 - LPF_FACTOR)) / 100;
        stick->rolling_y = fy;
        if (fy > 0) {
            fy = clamp(fy - stick->deadzone, 0, stick->max_y);
        } else if (fy < 0) {
            fy = clamp(fy + stick->deadzone, -stick->max_y, 0);
        }
        f32 ratio = ((f32) fy) / stick->max_y;
        return ratio * 127;
    }

    bool sw(Stick_t * stick) {
        return !digitalRead(stick->pin_sw);
    }

}