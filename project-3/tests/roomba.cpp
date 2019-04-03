#include <RTOS.h>
#include "Message.h"
#include "Peripherals.h"
#include "Roomba.h"

#include "Private.h"

#define SERIAL_BAUD 9600
#define ROOMBA_UART 2
#define BAUD_RATE_CHANGE_PIN 30
#define MAX_OVERRIDE_TIME 1000

// #define PRINT_SENSOR
#define PRINT_STATE
#define PRINT_DEATH

using namespace RTOS;

static bool override_control  = false;
static i64 override_time      = 0;
static i64 override_last_time = 0;

void wait_a_moment() {
    delay(100);
}

bool task_get_sensor_data_fn(Task_t * task) {
    if (Roomba::state == Move_State) {
        Roomba::get_sensor_data();
        if (Roomba::sensor_ir || Roomba::sensor_bumper) {
            // override_control = true;
            override_time = 0;
            override_last_time = Time::now();
        }
        #ifdef PRINT_SENSOR
            debug_print(
                "sensor_ir: %d sensor_bumper: %d\n", 
                (int) Roomba::sensor_ir, 
                (int) Roomba::sensor_bumper
            );
        #endif
    }
    return true;
}

bool task_mode_switch_fn(Task_t * task) {
    if (Roomba::state == Move_State) {
        Roomba::state = Still_State;
        Roomba::play_song(SONG_STILL);
        #ifdef PRINT_STATE
            debug_print("state: Still_State\n");
        #endif
    } else {
        Roomba::state = Move_State;
        Roomba::play_song(SONG_MOVE);
        #ifdef PRINT_STATE
            debug_print("state: Move_State\n");
        #endif
    }
    return true;
}

bool task_control_fn(Task_t * task) {
    
    // Check if we've been killed
    if (photocell_hit()) {
        #ifdef PRINT_DEATH
            debug_print("I am dead now.");
        #endif
        // maybe create a shut down task...
        // cleanup();
        set_laser(OFF);
        return false;
    }
    
    if (override_control && Roomba::state == Move_State) {
        if (override_time >= MAX_OVERRIDE_TIME) {
            override_control = false;
        } else {
            i64 now = Time::now();
            override_time += now - override_last_time;
            override_last_time = now;
            Roomba::move(0.0, -1.0);
        }
    }
    
    Message_t * current_message = Message::receive((Message_t *) task->state);
    
    if (current_message) {
        map_servo_pan(current_message->u_x, 0, STICK_U_OFFSET_X);
        map_servo_tilt(-current_message->u_y, 0, STICK_U_OFFSET_Y);
        if (!override_control) {
            debug_print("m_x: %d, m_y: %d\n", (int) current_message->m_x, (int) current_message->m_y);
            // Roomba::move(
            //     ((float) current_message->m_x) / Q78_to_int(STICK_M_MAX_X),
            //     ((float) current_message->m_y) / Q78_to_int(STICK_M_MAX_Y)
            //     // Q78_to_float( Q78_div(Q78(current_message->m_x), STICK_M_MAX_X)),
            //     // Q78_to_float(-Q78_div(Q78(current_message->m_y), STICK_M_MAX_Y))
            // );
        }
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
    
    // Initialize serial ports
    Serial1.begin(SERIAL_BAUD);
    
    wait_a_moment();

    // Clear any messages in the bluetooth buffer
    while (Serial1.available()) { Serial1.read(); }
    
    wait_a_moment();
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        Roomba::init(ROOMBA_UART, BAUD_RATE_CHANGE_PIN);
    }
        
    Task_t * task_control = Task::init("task_control", task_control_fn);
    task_control->period_ms = 60; //20;    
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