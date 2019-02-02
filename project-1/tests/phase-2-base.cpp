#include "Base.h"
#include "Phase-2.h"

/* pseduo-code

int g_done = NO;

Message current_message;

void sample() {
    if (photo_hit()) {
        current_message.done = YES;
    } else {
        current_message.u_x   = sample_stick_u_x();
        current_message.u_y   = sample_stick_u_y();
        current_message.m_x   = sample_stick_m_x();
        current_message.m_y   = sample_stick_m_y();
        current_message.laser = stick_u_down();
    }
}

void send() {
    // Write each byte of current message one at a time
    u8 * buffer = &current_message;
    int i;
    for (i = 0; i < sizeof(Message); i++) {
        Serial1.write(buffer[i]);
    }
}

*/
 
int main() {
    /* pseudo-code

    init_arduino();
    LiquidCcrystal lcd = init_lcd();
    init_stick_u_sw();
    Scheduler_Init();

    lcd.print("Not hit :)")

    // Scheduler
    Scheduler_StartTask(0, 10, sample);
    Scheduler_StartTask(5, 10, send);

    while (!g_done) {
        u32 idle_period = Scheduler_Dispatch();
        if (idle_period) {
            delay(idle_period);
        }
    }
    lcd.clear();
    lcd.print("Hit :O");
    */
   return 0;
}