#include "Base.h"
#include "Peripherals.h"

int main() {

    init_arduino();
    LiquidCrystal lcd = init_lcd();
    
    int x = -1;
    int y = -1;

    for(;;) {
        if (x != sample_stick_u_x() || y != sample_stick_u_y()) {
            x = sample_stick_u_x();
            y = sample_stick_u_y();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("x: ");
            lcd.print(x);
            lcd.setCursor(0, 1);
            lcd.print("y: ");
            lcd.print(y);
        }
    }
}