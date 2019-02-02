#include "Base.h"
#include "Peripherals.h"

int main() {
    Serial1.begin(SERIAL_BAUD);
    LiquidCrystal lcd = init_lcd();

    // Send the messagee
    const char * msg = "Hello!";
    Serial1.write(msg);
    lcd.print("sent: ");
    lcd.print(msg);
    lcd.setCursor(0, 1);
    lcd.print("recv: ");

    for (;;) {
        if (Serial1.available()) {
          lcd.write(Serial1.read());
        }
    }

    return 0;
}