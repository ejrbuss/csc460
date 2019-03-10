/**
 * schedule_17
 * 
 * Test:
 *  A periodic task can become an event-driven task.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

#define TIMER_COUNT 250

RTOS::Event_t e1;

ISR(TIMER3_COMPA_vect) {
    static int millis_passed = 0;
    millis_passed++;
    if (millis_passed == 1750) {
        RTOS::Event::dispatch(e1);
    }
}

void timer_init() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCCR3A = 0x00;                 // Clear control register A
        TCCR3B = 0x00;                 // Clear control register B
        TCNT3  = 0x00;                 // Clear the counter
        OCR3A  = TIMER_COUNT;          // The value we are waiting for
        TCCR3B |= BV(WGM32);           // Use CTC mode
        TCCR3B |= BV(CS31) | BV(CS30); // Scale by 64
        TIMSK3 |= BV(OCIE3A);          // Enable timer compare interrupt
    }
}

bool task_delayed_fn(RTOS::Task_t * task) {
    timer_init();   // start the timer
    return true;
}

bool task_periodic_fn(RTOS::Task_t * task) {
    static int count = 0;
    count++;
    if (count == 2) {
        task->period_ms = 0;
        task->events |= e1;
    }
    return true;
}

int main() {
    RTOS::init();
    
    e1 = RTOS::Event::init("event 1");
    
    RTOS::Task_t * task_delayed = RTOS::Task::init("task_delayed", task_delayed_fn);
    task_delayed->period_ms = 0;
    task_delayed->delay_ms = 0;
    
    RTOS::Task_t * task_periodic = RTOS::Task::init("task_periodic", task_periodic_fn);
    task_periodic->period_ms = 500;
    task_periodic->delay_ms = 500;
    
    RTOS::Task::dispatch(task_delayed);
    RTOS::Task::dispatch(task_periodic);
    
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {
    
    void trace(Trace_t * trace) { 
        Trace::serial_trace(trace);
        
        if (trace->tag == Mark_Start) {
            static int trace_no = 0;
            trace_no++;
            switch (trace_no) {
                case 2: {
                    i64 this_time = RTOS::Time::now();
                    assert(trace->mark.start.instance == 1 && this_time == 500);
                    break;
                }
                case 3: {
                    i64 this_time = RTOS::Time::now();
                    assert(trace->mark.start.instance == 1 && this_time == 1000);
                    break;
                }
                case 4: {
                    i64 this_time = RTOS::Time::now();
                    assert(trace->mark.start.instance == 1 && this_time >= 1750);
                    RTOS::debug_print("\nTest passed.\n");
                    RTOS::halt();
                    break;
                }
                default: 
                    break;
            }
        }
    }
    
    bool error(Trace_t * trace) { return true; }
    
}}