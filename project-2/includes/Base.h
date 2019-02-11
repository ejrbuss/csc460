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

#define SERIAL_BAUD 9600

void init_arduino();

#endif /* BASE_H */