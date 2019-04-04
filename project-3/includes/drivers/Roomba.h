/*
 * Adapted from Roomba2 and Neil's code. (make a good reference for this)
 */

#pragma once

#ifndef ROOMBA_H
#define ROOMBA_H

#include <RTOS.h>
#include "Compute.h"

namespace Roomba {

    typedef enum Song_t {
        Move_Song,
        Still_Song,
    } Song_t;

    typedef enum Roomba_State_t {
        Move_State,
        Still_State,
    } Roomba_State_t;

    extern Roomba_State_t state;
    
    void init(HardwareSerial * serial, int braud_pin);
    void move(i8 x, i8 y);
    void play_song(Song_t song);
    void load_song(Song_t song, u8 length, u8 * notes);

    namespace Sensors {
        extern bool ir;
        extern bool bumper;
        void update();
    }
    
}

#endif /* ROOMBA_H */