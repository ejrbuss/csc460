#include <RTOS.h>
#include "Message.h"
#include "Peripherals.h"
#include "Roomba.h"

#include "Private.h"

#define SERIAL_BAUD 9600
#define ROOMBA_UART 2
#define BAUD_RATE_CHANGE_PIN 30

// #define PRINT_SENSOR
#define PRINT_STATE
#define PRINT_DEATH

using namespace RTOS;

void wait_a_moment() {
    delay(100);
}

bool task_get_sensor_data_fn(Task_t * task) {
    Roomba::get_sensor_data();
    #ifdef PRINT_SENSOR
        debug_print(
            "sensor_ir: %d sensor_bumper: %d\n", 
            (int) Roomba::sensor_ir, 
            (int) Roomba::sensor_bumper
        );
    #endif
    return true;
}

bool task_mode_switch_fn(Task_t * task) {
    if (Roomba::state == Move_State) {
        Roomba::state = Still_State;
        #ifdef PRINT_STATE
            debug_print("state: Still_State");
        #endif
    } else {
        Roomba::state = Move_State;
        #ifdef PRINT_STATE
            debug_print("state: Move_State");
        #endif
    }
    Roomba::play_song(0);
    return true;
}

bool task_control_fn(Task_t * task) {
    
    // Check if we've been killed
    // if (photocell_hit()) {
    //     #ifdef PRINT_DEATH
    //         debug_print("I am dead now.");
    //     #endif
    //     // maybe create a shut down task...
    //     // cleanup();
    //     set_laser(OFF);
    //     return false;
    // }
    
    // Receive
    Message_t * current_message = Message::receive((Message_t *) task->state);
    
    // Control
    if (current_message) {
        map_servo_pan(current_message->u_x, 0, STICK_U_OFFSET_X);
        map_servo_tilt(-current_message->u_y, 0, STICK_U_OFFSET_Y);
        Roomba::send_command(current_message->m_x, current_message->m_y);
        set_laser(current_message->flags & MESSAGE_LASER);
        current_message = NULL;
    }
    
    task->state = (void *) current_message;
    return true;
}

int main() {
    RTOS::init();
    init_photocell();
    init_laser();
    init_servo_pan();
    init_servo_tilt();
    
    // set_laser(ON);  // for testing
    
    // Initialize serial ports
    Serial1.begin(SERIAL_BAUD);
    
    wait_a_moment();

    // Clear any messages in the bluetooth buffer
    while (Serial1.available()) { Serial1.read(); }
    
    wait_a_moment();
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        Roomba::init(ROOMBA_UART, BAUD_RATE_CHANGE_PIN);
    }
    
    Roomba::load_song();
    
    Task_t * task_control = Task::init("task_control", task_control_fn);
    task_control->period_ms = 20;    
    Task::dispatch(task_control);

    Task_t * task_mode_switch = Task::init("task_mode_switch", task_mode_switch_fn);
    task_mode_switch->period_ms = 30 * 1000;
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
        Trace::serial_trace(trace);
        if (trace->tag == Debug_Message) {
            Serial1.print(trace->debug.message);
        }
    }

    bool error(Trace_t * trace) {
        return true;
    }

}}