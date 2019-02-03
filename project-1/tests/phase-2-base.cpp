#define MESSAGE_SENDER

#include "Base.h"
#include "Phase-2.h"
#include "Peripherals.h"
#include "Scheduler.h"

int g_done = NO;

Message current_message;

void sample() {
    if (photocell_hit()) {
        g_done = YES;
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
    // Write each byte of current message onse at a time
    u8 * buffer = (u8 *) &current_message;
    u16 i;
    for (i = 0; i < sizeof(Message); i++) {
        Serial1.write(buffer[i]);
    }
}
 
int main() {
    init_arduino();
    init_stick_u_sw();
    Scheduler_Init();
    Serial1.begin(SERIAL_BAUD);

    LiquidCrystal lcd = init_lcd();
    lcd.print("Not hit :)");

    // We only set thi sonce
    current_message.header = MESSAGE_HEADER;

    // Scheduler
    Scheduler_StartTask(0, 24, sample);
    Scheduler_StartTask(10, 24, send);

    while (!g_done) {
        u32 idle_period = Scheduler_Dispatch();
        if (idle_period) {
            delay(idle_period);
        }
    }

    lcd.clear();
    lcd.print("Hit :O");
    // Send the goodbye message
    send();
    delay(1000);
    return 0;
}