#include "Base.h"
#include "Peripherals.h"
#include "Phase-2.h"
#include "Scheduler.h"

#define LOGIC_CONTROL 22
#define LOGIC_RECEIVE 24

int g_done = NO;

Message * current_message;
Message buffer_message;

void control() {
    digitalWrite(LOGIC_CONTROL, HIGH);
    if (current_message) {
        if (current_message->flags & MESSAGE_DONE) {
            // message_print(Serial, *current_message);
            g_done = YES;
        } else {
            map_servo_pan(current_message->u_x, 0, STICK_U_OFFSET_X);
            map_servo_tilt(-current_message->u_y, 0, STICK_U_OFFSET_Y);
            // map_roomba_x(current_message->m_x, 0, STICK_M_OFFSET_X);
            // map_roomba_y(current_message->m_y, 0, STICK_M_OFFSET_Y);
            set_laser(current_message->flags & MESSAGE_LASER);
            current_message = NULL;
        }
    }
    digitalWrite(LOGIC_CONTROL, LOW);
}

void receive() {

    static int i = 0;
    static int state = header1;

    digitalWrite(LOGIC_RECEIVE, HIGH);
    if (current_message == NULL) {
        u8 * buffer = (u8 *) &buffer_message;
        while (Serial1.available()) {
            switch(state) {
                case header1:
                case header2:
                case header3:
                case header4:
                    if (Serial1.read() == state) {
                        state++;
                    } else {
                        state = header1;
                    }
                    break;
                case data:
                    // Read as much as is available
                    while (Serial1.available()) {
                        buffer[i++] = Serial1.read();
                        if (i == sizeof(Message)) {
                            current_message = &buffer_message;
                            state = header1;
                            i = 0;
                            digitalWrite(LOGIC_RECEIVE, LOW);
                            return;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    digitalWrite(LOGIC_RECEIVE, LOW);
}

int main() {

    // Initialize hardware
    init_arduino();
    init_servo_pan();
    init_servo_tilt();
    init_laser();
    Scheduler_Init();

    pinMode(LOGIC_CONTROL, OUTPUT);
    pinMode(LOGIC_RECEIVE, OUTPUT);

    // Initialize serial ports
    Serial1.begin(SERIAL_BAUD);
    Serial.begin(SERIAL_BAUD);

    // Give things a moment
    delay(100);

    // Clear any messages in the bluetooth buffer
    while (Serial1.available()) { Serial1.read(); }

    // Schedule our tasks
    Scheduler_StartTask(0, 10, receive);
    Scheduler_StartTask(5, 10, control);

    u32 start = millis();
    u32 idle  = 0; 

    // Enter the main scheduler loop
    while (!g_done) {
        u32 idle_period = Scheduler_Dispatch();
        idle += idle_period;
        if (idle_period) {
            delay(idle_period);
        }
    }

    u32 finish = millis();
    Serial.print("idle: ");
    Serial.print(((double) idle) / ((double) finish - start));

    // We finished
    set_laser(OFF);
    // Ensure serial events finish
    delay(1000);
    return 0;
}