/**
 * schedule_12
 * 
 * Test:
 *  An interrupt goes off during a delayed task.
 *  The delayed task should finish and then the event-driven task should run.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

#define TIMER_COUNT 250

RTOS::Event_t e1;

ISR(TIMER3_COMPA_vect) {
    static int millis_passed = 0;
    millis_passed++;
    if (millis_passed == 10) {
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

bool task_event_1_fn(RTOS::Task_t * task) {
    return true;
}

bool task_delayed_fn(RTOS::Task_t * task) {
    timer_init();   // start the timer
    
    i64 now_time = RTOS::Time::now();
    RTOS::Time::idle(now_time, 30);
    while(RTOS::Time::now() - now_time < 30) {}
    return true;
}

bool task_delayed_end_fn(RTOS::Task_t * task) {
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 0,    "task_delayed",     },
        { 2000, "task_delayed_end", },
        { -1 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    
    e1 = RTOS::Event::init("event 1");
    RTOS::Task_t * task_event_1 = RTOS::Task::init("task_event_1", task_event_1_fn);
    task_event_1->events |= e1;
    
    RTOS::Task_t * task_delayed = RTOS::Task::init("task_delayed", task_delayed_fn);
    task_delayed->period_ms = 0;
    task_delayed->delay_ms = 0;
    
    RTOS::Task_t * task_delayed_end = RTOS::Task::init("task_delayed_end", task_delayed_end_fn);
    task_delayed_end->period_ms = 0;
    task_delayed_end->delay_ms = 2000;
    
    RTOS::Task::dispatch(task_event_1);
    RTOS::Task::dispatch(task_delayed);
    RTOS::Task::dispatch(task_delayed_end);
    
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {
    
    void trace(Trace_t * trace) { 
        Trace::serial_trace(trace);
        if (trace->tag != Debug_Message) {
            switch (trace->tag) {
                case Mark_Event: {
                    i64 this_time = RTOS::Time::now();
                    if(trace->mark.event.event == 0b1) {
                        assert(this_time >= 0 && this_time <= 30);
                    }
                    return;
                }
                case Mark_Start: {
                    i64 this_time = RTOS::Time::now();
                    if(trace->mark.start.instance == 0) {
                        assert(this_time >= 30);
                        return;
                    } else break;
                }
                default: 
                    break;
            }
        }
        Test::schedule_trace(trace);
    }
    
    bool error(Trace_t * trace) { return true; }
    
}}