#include "Base.h"
#include "Peripherals.h"
#include "Phase-2.h"
#include "Scheduler.h"

int g_done = NO;

Message * current_message;
Message buffer_message;

void control() {
    if (current_message) {
        if (current_message->done) {
            Serial.print("u_x: ");
            Serial.println(current_message->u_x);
            Serial.print("u_y: ");
            Serial.println(current_message->u_y);
            Serial.print("m_x: ");
            Serial.println(current_message->m_x);
            Serial.print("m_y: ");
            Serial.println(current_message->m_y);
            Serial.print("laser: ");
            Serial.println((int)current_message->laser);
            Serial.print("done: ");
            Serial.println((int)current_message->done);
            g_done = YES;
            return;
        }
        map_servo_pan(current_message->u_x, 0, STICK_U_OFFSET_X);
        map_servo_tilt(-current_message->u_y, 0, STICK_U_OFFSET_Y);
        // map_roomba_x(current_message->m_x, 0, STICK_M_OFFSET_X);
        // map_roomba_y(current_message->m_y, 0, STICK_M_OFFSET_Y);
        set_laser(current_message->laser);
        current_message = NULL;
    }
}

void receive() {
    static int i = 0;
    static int state = header1;

    while (current_message == NULL && Serial1.available()) {
        u8 * buffer = (u8 *) &buffer_message;
        int b = 0;

        switch(state) {
            case header1:
                if (Serial1.read() == 0x01) {
                    state = header2;
                } else {
                    state = header1;
                }
                break;
            case header2:
                if (Serial1.read() == 0x02) {
                    state = header3;
                } else {
                    state = header1;
                }
                break;
            case header3:
                if (Serial1.read() == 0x03) {
                    state = header4;
                } else {
                    state = header1;
                }
                break;
            case header4:
                if (Serial1.read() == 0x04) {
                    state = header5;
                } else {
                    state = header1;
                }
                break;
            case header5:
                if (Serial1.read() == 0x05) {
                    state = header6;
                } else {
                    state = header1;
                }
                break;
            case header6:
                if (Serial1.read() == 0x06) {
                    state = header7;
                } else {
                    state = header1;
                }
                break;
            case header7:
                if (Serial1.read() == 0x07) {
                    state = header8;
                } else {
                    state = header1;
                }
                break;
            case header8:
                if (Serial1.read() == 0x08) {
                    state = data;
                } else {
                    state = header1;
                }
                break;
            case data:
                // Read one byte
                buffer[i] = Serial1.read();
                i++;
                if (i == sizeof(Message)) {
                    current_message = &buffer_message;
                    i = 0;
                    state = header1;
                }
                break;
            default:
                break;
        }
    }
}

int main() {
    init_arduino();
    init_servo_pan();
    init_servo_tilt();
    init_laser();
    Scheduler_Init();
    Serial1.begin(SERIAL_BAUD);
    Serial.begin(SERIAL_BAUD);

    delay(1000);
    while (Serial1.available()) {
        Serial1.read();
    }

    // Scheduler
    Scheduler_StartTask(0, 1, control);
    Scheduler_StartTask(0, 1, receive);

    while (!g_done) {
        u32 idle_period = Scheduler_Dispatch();
        if (idle_period) {
            delay(idle_period);
        }
    }

    Serial.println("DONE!");
    set_laser(OFF);

    delay(1000);

   return 0;
}