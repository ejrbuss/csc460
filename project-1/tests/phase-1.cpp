#include "Base.h"
#include "Peripherals.h"

int main() {
    init_arduino();
    init_servo_pan();
    init_servo_tilt();
    init_stick_u_sw();
    init_laser();
    init_photocell();

    LiquidCrystal lcd = init_lcd();
    lcd.print("Not hit :)");

    for (;;) {
        map_servo_pan(sample_stick_u_x(), 0, STICK_U_OFFSET_X);
        map_servo_tilt(-sample_stick_u_y(), 0, STICK_U_OFFSET_Y);
        if (stick_u_down()) {
            set_laser(ON);
        } else {
            set_laser(OFF);
        }
        if (photocell_hit()) {
            lcd.clear();
            lcd.print("Hit :O");
            set_laser(OFF);
            break;
        }
    }
    return 0;
}