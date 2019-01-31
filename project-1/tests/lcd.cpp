#include <Arduino.h>

#include <Base.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

const int rs  = 8;
const int en  = 9;
const int db4 = 4;
const int db5 = 5;
const int db6 = 6;
const int db7 = 7;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
LiquidCrystal lcd(rs, en, db4, db5, db6, db7);

int main() {
    init_arduino();
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.print("hello, world!");
    for(;;) {
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.setCursor(0, 1);
        // print the number of seconds since reset:
        lcd.print(millis() / 1000);
    }
}