#pragma once

#ifndef TEST_H
#define TEST_H

#include "Base.h"
#include "Peripherals.h"

#define assert(cond) (Test::_assert(cond, (#cond)))

namespace Test {

    LiquidCrystal * lcd;

    void init() {
        init_arduino();
        Serial.begin(9600);
        lcd = init_lcd();
    }

    void _assert(bool cond, const char * cond_token) {
        if (cond) {
            lcd->print(".");
            Serial.print(".");
        } else {
            lcd->print("!");
            lcd->setCursor(0, 1);
            lcd->print(cond_token);
            Serial.print("!\nAssert failed: ");
            Serial.print(cond_token);
        }
    }

}

#endif /* TEST_H */