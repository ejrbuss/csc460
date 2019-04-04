#pragma once

#ifndef SERVO_H
#define SERVO_H

#include <RTOS.h>
#include <Servo.h>
#include "Compute.h"

typedef struct PTServo_t PTServo_t;
struct PTServo_t {
    i16 min;
    i16 max;
    i16 speed;
    i16 position;
    i64 last_call;
    Servo aservo;
};

namespace PTServo {
    PTServo_t * init_servo_pan();
    PTServo_t * init_servo_tilt();
    i16 control(PTServo_t * servo, i8 value);
}

#endif /* SERVO_H */