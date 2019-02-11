#define MESSAGE_SENDER

#include "Base.h"
#include "Phase-2.h"
#include "Peripherals.h"
#include "Scheduler.h"

#define LOGIC_SAMPLE 22
#define LOGIC_SEND   24

int g_done = NO;

Message current_message;

void sample() {
    digitalWrite(LOGIC_SAMPLE, HIGH);
    if (photocell_hit()) {
        g_done = YES;
        current_message.flags = MESSAGE_DONE;
    } else {
        current_message.u_x   = sample_stick_u_x();
        current_message.u_y   = sample_stick_u_y();
        current_message.m_x   = sample_stick_m_x();
        current_message.m_y   = sample_stick_m_y();
        current_message.flags = stick_u_down() ? MESSAGE_LASER : OFF;
    }
    digitalWrite(LOGIC_SAMPLE, LOW);
}

void send() {
    digitalWrite(LOGIC_SEND, HIGH);
    // Write each byte of current message onse at a time
    u8 * buffer = (u8 *) &current_message;
    u16 i;
    for (i = 0; i < sizeof(Message); i++) {
        Serial1.write(buffer[i]);
    }
    digitalWrite(LOGIC_SEND, LOW);
}
 
int main() {

    init_arduino();
    init_stick_u_sw();
    Scheduler_Init();
    
    // Setup Logic pins
    pinMode(LOGIC_SAMPLE, OUTPUT);
    pinMode(LOGIC_SEND, OUTPUT);

    Serial.begin(SERIAL_BAUD);
    Serial1.begin(SERIAL_BAUD);

    LiquidCrystal lcd = init_lcd();
    lcd.print("Not hit :)");

    // We only set thi sonce
    current_message.header = MESSAGE_HEADER;

    // Schedul tasks (we sample roughly 60 time a second)
    Scheduler_StartTask(0, 16, sample);
    Scheduler_StartTask(8, 16, send);

    u32 start = millis();
    u32 idle  = 0; 

    while (!g_done) {
        u32 idle_period = Scheduler_Dispatch();
        idle += idle_period;
        if (idle_period) {
            delay(idle_period);
        }
    }

    u32 finish = millis();
    lcd.clear();
    lcd.print("Hit :O");
    lcd.setCursor(0, 1);
    lcd.print("idle: ");
    lcd.print(((double) idle) / ((double) finish - start));
    // Send the goodbye message
    send();
    delay(1000);
    return 0;
}