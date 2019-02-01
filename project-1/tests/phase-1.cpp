#include "Base.h"
 
int main() {
    init_arduino();
    
    /* pseudo-code

    init_servo_pan();
    init_servo_tilt();
    init_stick_u_sw();

    lcd.print("Not hit :)");

    for (;;) {
        // Sample joystick
        int x = sample_stick_u_x();
        int y = sample_stick_u_y();

        // Move servos
        map_servo_pan(x, 0, STICK_U_OFFSET_X);
        map_servo_tilt(-y, 0, STICK_U_OFFSET_Y);

        // Enable/disable Laser
        if (stick_u_down()) {
            set_laser(ON);
        } else {
            set_laser(OFF);
        }

        if (photo_hit()) {
            lcd.clear();
            lcd.print("Hit :O");
            break; // Exit loop
        }
    }

    */
   return 0;
}