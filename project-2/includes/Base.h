#pragma once

#ifndef BASE_H
#define BASE_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "Arduino.h"
#include "Types.h"

#define TRUE  1
#define FALSE 0
#define YES   TRUE
#define NO    FALSE
#define ON    TRUE
#define OFF   FALSE

#define BV(bit) (1 << bit)

#ifdef LOGIC
    #define LOGIC_INIT(pin) pinMode(pin, OUTPUT);
    #define LOGIC_HIGH(pin) digitalWrite(pin, HIGH)
    #define LOGIC_LOW(pin)  digitalWrite(pin, LOW)
#else
    #define LOGIC_INIT(pin) ;
    #define LOGIC_HIGH(pin) ;
    #define LOGIC_LOW(pin)  ;
#endif

#define SERIAL_BAUD 9600

void init_arduino();

#endif /* BASE_H */