#include <RTOS.h>
#include "Message.h"
#include "Peripherals.h"
#include "Roomba.h"

#define SERIAL_BAUD 9600
#define ROOMBA_UART 2
#define BAUD_RATE_CHANGE_PIN 30
#define THIRTY_SEC_IN_MS 30000
#define TIMER_COUNT 31250

volatile ROOMBA_STATE rstate = SAFE_MODE;
int g_done = 0;
bool hit = false;
volatile bool dead = false;


ISR(TIMER3_COMPA_vect) {
    dead = true;
}

void reset_timer() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCCR3A = 0x00;                 // Clear control register A
        TCCR3B = 0x00;                 // Clear control register B
        TCNT3  = 0x00;                 // Clear the counter
        OCR3A  = TIMER_COUNT;          // The value we are waiting for
        TCCR3B |= BV(CS32) | BV(CS30); // Scale by 1024
    }    
}

void stop_timer() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCCR3B &= 0b11111000;   // Select no clock source
    }    
}

void timer_init() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCCR3A = 0x00;                 // Clear control register A
        TCCR3B = 0x00;                 // Clear control register B
        TCNT3  = 0x00;                 // Clear the counter
        OCR3A  = TIMER_COUNT;          // The value we are waiting for
        TCCR3B |= BV(WGM32);           // Use CTC mode
        // TCCR3B |= BV(CS31) | BV(CS30); // Scale by 64
        TIMSK3 |= BV(OCIE3A);          // Enable timer compare interrupt
    }
}

bool task_photocell_fn(RTOS::Task_t * task) {
    hit = false;
    dead = false;
    
    for(;;) {
        set_laser(1);
        if (dead) {
            Serial1.println("We're dead :(");
            break;
        }
        if (photocell_hit()) {
            if (!hit) {
                hit = true;
                Serial1.println("started timer");
                reset_timer();
            }
        } else {
            hit = false;
            Serial1.println("stopped timer");
            stop_timer();
        }
    }
    
    return true;
}

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
    RTOS::Task_t * task_photocell = RTOS::Task::init("task_photocell", task_photocell_fn);
    
    task_control->period_ms = 60; //20;
    task_control->state = NULL;
    
    task_mode_switch->period_ms = THIRTY_SEC_IN_MS;
    
    task_photocell->period_ms = 0;
    task_photocell->delay_ms = 0;
    
    // RTOS::Task::dispatch(task_control);
    // RTOS::Task::dispatch(task_mode_switch);
    RTOS::Task::dispatch(task_photocell);
    
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