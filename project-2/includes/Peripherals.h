#pragma once

#include "Base.h"
#include "Compute.h"
#include "LiquidCrystal.h"

#ifndef PERIPHERALS_H
#define PERIPHERALS_H

//
// LASER
//

#define LASER_PIN 52

void init_laser();
void toggle_laser();
void set_laser(int on);
int laser_on();

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
// Sticks 
//

#define STICK_SCALE      8
#define STICK_LFP_FACTOR Q78(0.1)

// 
// STICK_U
//

#define STICK_U_PIN_X 8
#define STICK_U_PIN_Y 9
#define STICK_U_PIN_SW 50

#define STICK_U_OFFSET_X 506
#define STICK_U_OFFSET_Y 506
#define STICK_U_DEADZONE (50 / STICK_SCALE)

#define STICK_U_MAX_X    ((STICK_U_OFFSET_X / STICK_SCALE) - (2 * STICK_U_DEADZONE))
#define STICK_U_MIN_X    (-STICK_U_MAX_X)

#define STICK_U_MAX_Y    ((STICK_U_OFFSET_Y / STICK_SCALE) - (2 * STICK_U_DEADZONE))
#define STICK_U_MIN_Y    (-STICK_U_MAX_Y)

int sample_stick_u_x();
int sample_stick_u_y();
void init_stick_u_sw();
int stick_u_down();
void stick_u_on_switch(void (*isr)());

// 
// STICK_M
//

#define STICK_M_PIN_X 8
#define STICK_M_PIN_Y 9
#define STICK_M_PIN_SW 2

#define STICK_M_SW_DELAY 30

#define STICK_M_OFFSET_X 506
#define STICK_M_OFFSET_Y 506
#define STICK_M_DEADZONE (100 / STICK_SCALE)

#define STICK_M_MAX_X    (Q78(STICK_U_OFFSET_X / STICK_SCALE) - STICK_U_DEADZONE)
#define STICK_M_MIN_X    (-STICK_U_MAX_X)

#define STICK_M_MAX_Y    (Q78(STICK_U_OFFSET_X / STICK_SCALE) - STICK_U_DEADZONE)
#define STICK_M_MIN_Y    (-STICK_U_MAX_X)

int sample_stick_m_x();
int sample_stick_m_y();
void stick_m_on_switch(void (*isr)());

//
// PHOTOCELL
//

#define PHOTO_PIN 11

#define PHOTO_HIT_THRESHOLD 1000

void init_photocell();
int photocell_hit();

#endif /* PERIPHERALS_H */