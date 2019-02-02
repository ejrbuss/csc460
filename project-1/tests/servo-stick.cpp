#include "Base.h"
#include "Peripherals.h"

int main() {
    init_arduino();
    init_servo_pan();
    init_servo_tilt();
    // attaches the servo on pin 9 to the servo object
    for (;;) {
        int x = sample_stick_u_x();
        int y = sample_stick_u_y();
        map_servo_pan(x, 0, STICK_U_MAX_X);
        map_servo_tilt(-y, 0, STICK_U_MAX_Y);
    }
}