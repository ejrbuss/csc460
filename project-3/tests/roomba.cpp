#include <RTOS.h>
#include "Message.h"
#include "Peripherals.h"
#include "Roomba.h"

#define SERIAL_BAUD 9600
#define ROOMBA_UART 2
#define BAUD_RATE_CHANGE_PIN 30
#define THIRTY_SEC_IN_MS 30000

volatile ROOMBA_STATE rstate = SAFE_MODE;

bool task_mode_switch_fn(RTOS::Task_t * task) {
    if (rstate == SAFE_MODE) {
        rstate = PASSIVE_MODE;
    } else {
        rstate = SAFE_MODE;
    }
    return true;
}

bool task_control_fn(RTOS::Task_t * task) {
    // Set the state of the Roomba
    Roomba::set_state(rstate);
    
    // Check if we've been killed
    if (photocell_hit()) {
        Serial1.println("Turning off.");
        // maybe create a shut down task...
        return false;
    }
    
    // Receive
    Message_t * current_message = Message::receive((Message_t *) task->state);
    
    // Control
    if (current_message) {
        if (current_message->flags & MESSAGE_DONE) {
            // message_print(Serial, *current_message);
        } else {
            map_servo_pan(current_message->u_x, 0, STICK_U_OFFSET_X);
            map_servo_tilt(-current_message->u_y, 0, STICK_U_OFFSET_Y);
            Roomba::send_command(current_message->m_x, current_message->m_y);
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
    init_photocell();
    init_laser();
    timer_init();
    
    // Initialize serial ports
    Serial1.begin(SERIAL_BAUD);
    
    // Give things a moment
    delay(100);

    // Clear any messages in the bluetooth buffer
    while (Serial1.available()) { Serial1.read(); }
    
    delay(100);
    
    Roomba::configure(ROOMBA_UART, BAUD_RATE_CHANGE_PIN);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        Roomba::init();
    }
    
    RTOS::Task_t * task_control = RTOS::Task::init("task_control", task_control_fn);
    RTOS::Task_t * task_mode_switch = RTOS::Task::init("task_mode_switch", task_mode_switch_fn);
    
    task_control->period_ms = 60; //20;
    task_control->state = NULL;
    
    task_mode_switch->period_ms = THIRTY_SEC_IN_MS;
    
    RTOS::Task::dispatch(task_control);
    RTOS::Task::dispatch(task_mode_switch);
    
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