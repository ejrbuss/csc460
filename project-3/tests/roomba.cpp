#include <RTOS.h>
#include "Message.h"
#include "Peripherals.h"
#include "Roomba.h"

#define BAUD 9600
#define ROOMBA_UART 2
#define BAUD_RATE_CHANGE_PIN 30

int g_done = 0;

bool task_control_fn(RTOS::Task_t * task) {
    Message_t * current_message = (Message_t *) task->state;
    
    if (current_message) {
        if (current_message->flags & MESSAGE_DONE) {
            // message_print(Serial, *current_message);
            g_done = 1;
        } else {
            map_servo_pan(current_message->u_x, 0, STICK_U_OFFSET_X);
            map_servo_tilt(-current_message->u_y, 0, STICK_U_OFFSET_Y);
            // Roomba::send_command(current_message->m_x, current_message->m_y);
            // map_roomba_x(current_message->m_x, 0, STICK_M_OFFSET_X);
            // map_roomba_y(current_message->m_y, 0, STICK_M_OFFSET_Y);
            set_laser(current_message->flags & MESSAGE_LASER);
            current_message = NULL;
        }
    }
    task->state = (void *) current_message;
    return true;
}

bool task_recv_message_fn(RTOS::Task_t * task) {
    task->state = (void *) Message::receive((Message_t *) task->state);
    return true;
}


int main() {
    RTOS::init();

    // Initialize serial ports
    Serial1.begin(BAUD);
    
    // Give things a moment
    delay(100);

    // Clear any messages in the bluetooth buffer
    while (Serial1.available()) { Serial1.read(); }
    
    Message_t * current_message = NULL;
    
    Roomba::configure(ROOMBA_UART, BAUD_RATE_CHANGE_PIN);
    Roomba::init();
    
    RTOS::Task_t * task_recv_message = RTOS::Task::init("task_recv_message", task_recv_message_fn);
    RTOS::Task_t * task_control = RTOS::Task::init("task_control", task_control_fn);
    
    task_recv_message->period_ms = 10;
    task_recv_message->state = (void *) current_message;
    
    task_control->period_ms = 10;
    task_control->delay_ms = 5;
    
    RTOS::Task::dispatch(task_recv_message);
    RTOS::Task::dispatch(task_control);
    RTOS::dispatch();
    
    // bool initialized = true;
    //
    // // Read commands from Serial.
    // while (1) {
    //     if(Serial1.available()) {
    //
    //         if(!initialized) {
    //             Roomba::init();
    //             initialized = true;
    //         }
    //
    //         // Read in a command from serial and send it to the Roomba
    //         int command = Serial1.read();
    //         switch(command) {
    //             case 'f': 
    //                 Roomba::drive(150, 32768);
    //                 break;
    //             case 'b':
    //                 Roomba::drive(-150, 32768);
    //                 break;
    //             case 'r':
    //                 Roomba::drive(50, -1);
    //                 break;
    //             case 'l':
    //                 Roomba::drive(50, 1);
    //                 break;
    //             case 's':
    //                 Roomba::drive(0,0);
    //                 break;
    //             case 'd':
    //                 Roomba::dock();
    //                 break;
    //             case 'p':
    //                 Roomba::power_off();
    //                 initialized = false;
    //                 break;
    //             default:
    //                 break;
    //         }
    //     }
    // 
    //     delay(100);
    // }
    
    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * trace) {
        return;
    }

    bool error(Trace_t * trace) {
        return true;
    }

}}