#pragma once

#ifndef STICK_H
#define STICK_H

#include <RTOS.h>

typedef struct Stick_t Stick_t;
struct Stick_t {
    u8 pin_x;
    u8 pin_y;
    u8 pin_sw;
    i16 offset_x;
    i16 offset_y;
    i16 deadzone;
    i16 max_x;
    i16 max_y;
    i16 rolling_x;
    i16 rolling_y;
};

namespace Stick {

    Stick_t * init_stick_u();
    Stick_t * init_stick_m();

    i8 x(Stick_t * stick);
    i8 y(Stick_t * stick);
    bool sw(Stick_t * stick);

}

#endif /* STICK_H */