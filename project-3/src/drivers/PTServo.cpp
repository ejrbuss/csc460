#include "PTServo.h"

#define DELAY 5

namespace PTServo {

    PTServo_t * init_servo_pan() {
        PTServo_t * servo_pan = (PTServo_t *) RTOS::Memory::static_alloc("servo_pan", sizeof(PTServo_t));
        servo_pan->last_call  = 0; 
        servo_pan->min        = 500;
        servo_pan->max        = 2000;
        servo_pan->speed      = 15;
        servo_pan->position   = 1500;
        servo_pan->aservo     = Servo();
        servo_pan->aservo.attach(13);
        servo_pan->aservo.writeMicroseconds(servo_pan->position);
        return servo_pan;
    }

    PTServo_t * init_servo_tilt() {
        PTServo_t * servo_tilt = (PTServo_t *) RTOS::Memory::static_alloc("servo_tilt", sizeof(PTServo_t));
        servo_tilt->last_call  = 0; 
        servo_tilt->min        = 750;
        servo_tilt->max        = 1250;
        servo_tilt->speed      = 10;
        servo_tilt->position   = 1250;
        servo_tilt->aservo     = Servo();
        servo_tilt->aservo.attach(12);
        servo_tilt->aservo.writeMicroseconds(servo_tilt->position);
        return servo_tilt;
    }

    i16 control(PTServo_t * servo, i8 value) {
        // Time management (don't over turn the servo)
        i64 this_call = RTOS::Time::now();
        if (this_call - servo->last_call < DELAY) {
            return servo->position;
        }
        servo->last_call = this_call;

        // Calculate the new position
        i16 speed    = servo->speed;
        i16 delta    = clamp((value * speed) / 127, -speed, speed);
        i16 position = clamp(servo->position + delta, servo->min, servo->max);

        // Update the servo
        servo->aservo.writeMicroseconds(position);
        servo->position = position;
        return position;
    }

}