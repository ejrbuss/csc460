#include <Arduino.h>

#include <Base.h>
#include <Compute.h>
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
    lcd.print("sizeof int: ");
    lcd.print(sizeof(int));
    lcd.setCursor(0, 1);
    lcd.print("sizof long: ");
    lcd.print(sizeof(long));

    Q78_t a;
    Q78_t b;
    int c;

    for (;;) {
        lcd.clear();
        lcd.print("0.5 + 0.5 = ");
        a = Q78(0.5);
        b = Q78(0.5);
        c = Q78_to_int(a + b);
        lcd.print(c);
        delay(2000);

        lcd.clear();
        lcd.print("4.5 * 12 = ");
        a = Q78(4.5);
        b = Q78(12.0);
        c = Q78_to_int(Q78_mul(a, b));
        lcd.print(c);
        delay(2000);

        lcd.clear();
        lcd.print("100 / 8 = ");
        a = Q78(100);
        b = Q78(8);
        c = Q78_to_int(Q78_div(a, b));
        lcd.print(c);
        delay(2000);
    }
    
}