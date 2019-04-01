#include <RTOS.h>
#include "Message.h"
#include "Peripherals.h"
#include "Roomba.h"

#define SERIAL_BAUD 9600
#define ROOMBA_UART 2
#define BAUD_RATE_CHANGE_PIN 30
#define THIRTY_SEC_IN_MS 30000

using namespace RTOS;

bool task_get_sensor_data_fn(Task_t * task) {
    Roomba::get_sensor_data();
    Serial1.print("sensor_ir: ");
    Serial1.println(Roomba::sensor_ir);
    Serial1.print("sensor_bumper: ");
    Serial1.println(Roomba::sensor_bumper);
    return true;
}

bool task_mode_switch_fn(Task_t * task) {
    if (Roomba::state == Move_State) {
        Roomba::state = Still_State;
    } else {
        Roomba::state = Move_State;
    }
    Roomba::play_song(0);
    return true;
}

bool task_control_fn(Task_t * task) {
    
    // Check if we've been killed
    if (photocell_hit()) {
        Serial1.println("Turning off.");
        // maybe create a shut down task...
        set_laser(OFF);
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
    
    set_laser(ON);  // for testing
    
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
    
    Roomba::load_song();
    
    Task_t * task_control = Task::init("task_control", task_control_fn);
    task_control->period_ms = 60; // 20;    
    Task::dispatch(task_control);

    Task_t * task_mode_switch = Task::init("task_mode_switch", task_mode_switch_fn);
    task_mode_switch->period_ms = THIRTY_SEC_IN_MS;
    Task::dispatch(task_mode_switch);

    Task_t * task_get_sensor_data = Task::init("task_get_sensor_data", task_get_sensor_data_fn);
    task_get_sensor_data->period_ms = 600;
    task_get_sensor_data->delay_ms  = 30;
    Task::dispatch(task_get_sensor_data);
    
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