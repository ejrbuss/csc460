#include <Arduino.h>

#include "Base.h"
#include "Peripherals.h"

#define LOW_PASS_FACTOR 0.1
#define STICK_FACTOR 0.1
#define STICK_MAX 43

const int rs  = 8;
const int en  = 9;
const int db4 = 4;
const int db5 = 5;
const int db6 = 6;
const int db7 = 7;

const int ax = 1;
const int ay = 2;

int main() {

    init_arduino();
    LiquidCrystal lcd = init_lcd();
    
    int x = -1;
    int y = -1;

    for(;;) {
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        if (x != sample_stick_u_x() || y != sample_stick_u_y()) {
            x = sample_stick_u_x();
            y = sample_stick_u_y();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("x: ");
            lcd.print(x);
            lcd.print(" alg:");
            lcd.print((analogRead(STICK_U_PIN_X) - STICK_U_OFFSET_X) / STICK_SCALE);
            lcd.setCursor(0, 1);
            lcd.print("y: ");
            lcd.print(y);
            lcd.print(" alg:");
            lcd.print((analogRead(STICK_U_PIN_Y) - STICK_U_OFFSET_Y) / STICK_SCALE);
        }
    }
}