#include <RTOS.h>
#include "Peripherals.h"
#include "Message.h"
#include "PTServo.h"
#include "Roomba.h"


#define BLUETOOTH_BAUD       9600
#define BAUD_RATE_CHANGE_PIN 30
#define MAX_OVERRIDE_TIME    250

// #define PRINT_SENSOR
// #define PRINT_XY
// #define PRINT_SERVO
#define PRINT_STATE
#define PRINT_DEATH

using namespace RTOS;

static HardwareSerial * bluetooth;
static bool override_control  = false;
static i64 override_time      = 0;
static i64 override_last_time = 0;

static PTServo_t * servo_pan;
static PTServo_t * servo_tilt;

bool task_get_sensor_data_fn(Task_t * task) {
    if (Roomba::state == Roomba::Move_State) {
        Roomba::Sensors::update();
        if (Roomba::Sensors::ir || Roomba::Sensors::bumper) {
            override_control = true;
            override_time = 0;
            override_last_time = Time::now();
        }
        #ifdef PRINT_SENSOR
            debug_print(
                "Roomba::Sensors::ir = %d\nRoomba::Sensors::bumper = %d\n",
                (int) Roomba::Sensors::ir, 
                (int) Roomba::Sensors::bumper
            );
        #endif
    }
    return true;
}

bool task_mode_switch_fn(Task_t * task) {
    if (Roomba::state == Roomba::Move_State) {
        override_control = false;
        Roomba::state = Roomba::Still_State;
        Roomba::play_song(Roomba::Still_Song);
        #ifdef PRINT_STATE
            debug_print("Roomba::state = Still_State\n");
        #endif
    } else {
        override_control = false;
        Roomba::state = Roomba::Move_State;
        Roomba::play_song(Roomba::Move_Song);
        #ifdef PRINT_STATE
            debug_print("Roomba::state = Move_State\n");
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
        set_laser(OFF);
        return false;
    }
    
    if (override_control) {
        if (override_time >= MAX_OVERRIDE_TIME) {
            override_control = false;
        } else {
            i64 now = Time::now();
            override_time += now - override_last_time;
            override_last_time = now;
            Roomba::move(0.0, -127);
        }
    }
    
    Message_t * current_message = Message::receive((Message_t *) task->state);
    
    if (current_message) {

        #ifdef PRINT_XY
            debug_print(
                "m_x = %d m_y = %d u_x = %d u_y = %d\n",
                (int) current_message->m_x,
                (int) current_message->m_y,
                (int) current_message->u_x,
                (int) current_message->u_y
            );
        #endif

        if (!override_control) {
            Roomba::move(current_message->m_x, -current_message->m_y);
        }

        #ifdef PRINT_SERVO
            i16 pan_position  = PTServo::control(servo_pan, current_message->u_x);
            i16 tilt_position = PTServo::control(servo_tilt, current_message->u_y);
            debug_print(
                "pan: %d\ttilt: %d\n",
                (int) pan_position,
                (int) tilt_position
            );
        #else
            PTServo::control(servo_pan, current_message->u_x);
            PTServo::control(servo_tilt, current_message->u_y);
        #endif

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

    // Might help with twitchy Servo
    TIMSK0 = 0;
    
    // Initialize serial ports
    bluetooth = &Serial1;
    bluetooth->begin(BLUETOOTH_BAUD);
    servo_pan  = PTServo::init_servo_pan();
    servo_tilt = PTServo::init_servo_tilt();
    
    // Clear any messages in the bluetooth buffer
    while (bluetooth->available()) { bluetooth->read(); }
    
    
    Roomba::init(&Serial2, BAUD_RATE_CHANGE_PIN);
        
    Task_t * task_control = Task::init("task_control", task_control_fn);
    task_control->period_ms = 60; // 20;    
    task_control->delay_ms  = 100;
    Task::dispatch(task_control);

    Task_t * task_mode_switch = Task::init("task_mode_switch", task_mode_switch_fn);
    task_mode_switch->period_ms = 30 * 1000;
    task_mode_switch->delay_ms  = 100;
    Task::dispatch(task_mode_switch);

    Task_t * task_get_sensor_data = Task::init("task_get_sensor_data", task_get_sensor_data_fn);
    task_get_sensor_data->period_ms = 120;
    task_get_sensor_data->delay_ms  = 130;
    Task::dispatch(task_get_sensor_data);
    
    RTOS::dispatch();

    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * trace) {
        Trace::serial_trace(trace);
        if (trace->tag == Debug_Message) {
            bluetooth->print(trace->debug.message);
        }
    }

    bool error(Trace_t * trace) {
        return true;
    }

}}