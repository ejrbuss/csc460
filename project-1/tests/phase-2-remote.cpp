#include "Base.h"
#include "Phase-2.h"

/* pseduo-code

int g_done = NO;

Message * current_message;
Message buffer_messsage;

void control() {
    if (current_message) {
        if (message.done) {
            g_done = YES:
            return;
        }
        map_servo_pan(current_message->u_x, 0, STICK_U_OFFSET_X);
        map_servo_tilt(current_message->u_y, 0, STICK_U_OFFSET_Y);
        map_roomba_x(current_message->m_x, 0, STICK_M_OFFSET_X);
        map_roomba_y(current_message->m_y, 0, STICK_M_OFFSET_Y);
        set_laser(current_message->laser);
        current_message = NULL;
    }
}

void recieve() {
    static int i = 0;
    if (current_message == NULL) {
        char * buffer = (void *) &buffer_messsage;
        while (Serial1.available()) {
            buffer[i] = Serial1.read();
            i++;
            if (i == sizeof(Message)) {
                current_message = &buffer_messsage;
                return;
            }
        }
    }
}

*/
 
int main() {
    /* pseudo-code

    init_arduino();
    init_stick_u_sw();
    Scheduler_Init();

    // Scheduler
    Scheduler_StartTask(0, 10, control);
    Scheduler_StartTask(0, 5, recieve);

    while (!g_done) {
        u32 idle_period = Scheduler_Dispatch();
        if (idle_period) {
            idle(idle_period);
        }
    }

    */
   return 0;
}