#include "Base.h"
#include "Compute.h"
#include "Peripherals.h"

int main() {
    init_arduino();
    LiquidCrystal lcd = init_lcd();

    Q78_t a;
    Q78_t b;
    int c;

    for (;;) {
        lcd.clear();
        lcd.print("sizeof int: ");
        lcd.print(sizeof(int));
        lcd.setCursor(0, 1);
        lcd.print("sizof long: ");
        lcd.print(sizeof(long));
        delay(2000);

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