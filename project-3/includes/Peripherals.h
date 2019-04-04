#pragma once

#include "Servo.h"
#include "Compute.h"
#include "LiquidCrystal.h"
#include "RTOS.h"

#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#define TRUE  1
#define FALSE 0
#define YES   TRUE
#define NO    FALSE
#define ON    TRUE
#define OFF   FALSE

//
// LASER
//

#define LASER_PIN 52

void init_laser();
void set_laser(int on);

//
// LCD
//

#define LCD_RS  8
#define LCD_EN  9
#define LCD_DB4 4
#define LCD_DB5 5
#define LCD_DB6 6
#define LCD_DB7 7

LiquidCrystal init_lcd();

//
// SERVO_PAN
//

#define SERVO_PAN_PIN       13
#define SERVO_PAN_TOP       2000
#define SERVO_PAN_BOTTOM    500
#define SERVO_PAN_CENTER    1500
#define SERVO_PAN_MAX_SPEED 7
#define SERVO_PAN_DELAY     5

void init_servo_pan();
void map_servo_pan(int value, int min_value, int max_value);

// 
// SERVO_TILT
//

#define SERVO_TILT_PIN       12
#define SERVO_TILT_TOP       1250
#define SERVO_TILT_BOTTOM    750
#define SERVO_TILT_CENTER    1000
#define SERVO_TILT_MAX_SPEED 5
#define SERVO_TILT_DELAY     5

void init_servo_tilt();
void map_servo_tilt(int value, int min_value, int max_value);

//
// PHOTOCELL
//

#define PHOTO_PIN 11

#define PHOTO_HIT_THRESHOLD 400

void init_photocell();
bool photocell_hit();

#endif /* PERIPHERALS_H */