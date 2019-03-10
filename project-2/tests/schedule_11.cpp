/**
 * schedule_11
 * 
 * Test:
 *  An interrupt goes off during a periodic task.
 *  The periodic task should finish and then the event-driven task should run, 
 *  given that there is enough time for it to finish before the periodic task
 *  goes off again.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

#define TIMER_COUNT 250

RTOS::Event_t e1;

ISR(TIMER3_COMPA_vect) {
    static int millis_passed = 0;
    millis_passed++;
    if (millis_passed == 502) {
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
    return true;
}

bool task_periodic_fn(RTOS::Task_t * task) {
    i64 now_time = RTOS::Time::now();
    RTOS::Time::idle(now_time, 30);
    while(RTOS::Time::now() - now_time < 30) {}
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 0,    "task_delayed",  },
        { 500,  "task_periodic", },
        { 1000, "task_periodic", },
        { 1501, "task_periodic", }, // { 1500, "task_periodic", }, Too slow because of printing
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
    
    RTOS::Task_t * task_periodic = RTOS::Task::init("task_periodic", task_periodic_fn);
    task_periodic->period_ms = 500;
    task_periodic->delay_ms = 500;
    
    RTOS::Task::dispatch(task_event_1);
    RTOS::Task::dispatch(task_delayed);
    RTOS::Task::dispatch(task_periodic);
    
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {
    
    void trace(Trace_t * trace) { 
        Trace::serial_trace(trace);
        switch (trace->tag) {
            case Mark_Event: {
                if(trace->mark.event.event == e1) {
                    assert(trace->mark.time >= 500 && trace->mark.time <= 530);
                }
                return;
            }
            case Mark_Start: {
                if(trace->mark.start.instance == 0) {
                    assert(trace->mark.time < 1000 && trace->mark.time >= 530);
                    return;
                }
            }
            default: 
                break;
        }
        Test::schedule_trace(trace);
    }
    
    bool error(Trace_t * trace) { return true; }
    
}}