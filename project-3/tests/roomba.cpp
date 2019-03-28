#include <RTOS.h>
#include "Message.h"
#include "Peripherals.h"
#include "Roomba.h"

#define SERIAL_BAUD 9600
#define ROOMBA_UART 2
#define BAUD_RATE_CHANGE_PIN 30

int g_done = 0;

bool task_control_fn(RTOS::Task_t * task) {
    // Receive
    Message_t * current_message = Message::receive((Message_t *) task->state);
    
    // Control
    if (current_message) {
        if (current_message->flags & MESSAGE_DONE) {
            // message_print(Serial, *current_message);
            g_done = 1;
        } else {
            map_servo_pan(current_message->u_x, 0, STICK_U_OFFSET_X);
            map_servo_tilt(-current_message->u_y, 0, STICK_U_OFFSET_Y);
            Roomba::send_command(current_message->m_x);
            // map_roomba_x(current_message->m_x, 0, STICK_M_OFFSET_X);
            // map_roomba_y(current_message->m_y, 0, STICK_M_OFFSET_Y);
            set_laser(current_message->flags & MESSAGE_LASER);
            current_message = NULL;
        }
    }
    
    task->state = (void *) current_message;
    return true;
}

int main() {
    RTOS::init();

    // Initialize serial ports
    Serial1.begin(SERIAL_BAUD);
    
    // Give things a moment
    delay(100);

    // Clear any messages in the bluetooth buffer
    while (Serial1.available()) { Serial1.read(); }
    
    delay(100);
    
    Roomba::configure(ROOMBA_UART, BAUD_RATE_CHANGE_PIN);
    Roomba::init();
    
    RTOS::Task_t * task_control = RTOS::Task::init("task_control", task_control_fn);
    
    task_control->period_ms = 20;
    task_control->state = NULL;
    
    RTOS::Task::dispatch(task_control);
    RTOS::dispatch();
    
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