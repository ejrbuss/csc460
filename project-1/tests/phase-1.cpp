#include "Base.h"
#include "Peripherals.h"
 
int main() {
    init_arduino();
    init_servo_pan();
    init_servo_tilt();
    init_stick_u_sw();
    init_laser();
    init_photocell();

    auto lcd = init_lcd();
    lcd.print("Not hit :)");

    for (;;) {
        // Sample joystick & Move servos
        map_servo_pan(sample_stick_u_x(), 0, STICK_U_OFFSET_X);
        map_servo_tilt(-sample_stick_u_y(), 0, STICK_U_OFFSET_Y);

        // Enable/disable Laser
        if (stick_u_down()) {
            set_laser(ON);
        } else {
            set_laser(OFF);
        }

        // Check if the ship is going down captain!
        if (photocell_hit()) {
            lcd.clear();
            lcd.print("Hit :O");
            break; // Exit loop
        }
    }
    return 0;
}