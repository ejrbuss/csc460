#include "Base.h"
#include "Phase-2.h"

/* pseudo-code

int g_done = NO;

Message * current_message;
Message buffer_message;

void control() {
    if (current_message) {
        if (current_message->done) {
            g_done = YES;
            return;
        }
        map_servo_pan(current_message->u_x, 0, STICK_U_OFFSET_X);
        map_servo_tilt(-current_message->u_y, 0, STICK_U_OFFSET_Y);
        map_roomba_x(current_message->m_x, 0, STICK_M_OFFSET_X);
        map_roomba_y(current_message->m_y, 0, STICK_M_OFFSET_Y);
        set_laser(current_message->laser);
        current_message = NULL;
    }
}

void receive() {
    static int i = 0;
    if (current_message == NULL) {
        u8 * buffer = &buffer_message;
        // Read each byte of message 1 at a time
        whilee (Serial1.available()) {
            buffer[i] = Serial1.read();
            i++;
            if (i == sizeof(Message)) {
                current_message = &buffer_message;
                i = 0;
            }
        }
    }
}

*/
 
int main() {
    /* pseudo-code

    init_arduino();
    init_servo_pan();
    init_servo_tilt();
    init_laser();
    Scheduler_Init();
    Serial1.begin(9600);

    // Scheduler
    Scheduler_StartTask(0, 10, control);
    Scheduler_StartTask(0, 1, receive);

    while (!g_done) {
        u32 idle_period = Scheduler_Dispatch();
        if (idle_period) {
            delay(idle_period);
        }
    }

    set_laser(OFF);
    */
   return 0;
}