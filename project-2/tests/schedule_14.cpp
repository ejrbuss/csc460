
/**
 * schedule_13
 * 
 * Test:
 *  Within one large idle time, an interrupt goes off twice and the event-driven task is ran twice.
 *   Sequence of events:
 *   - periodic task runs
 *   - interrupt goes off
 *   - periodic task finishes
 *   - event driven task goes off because the OS doesn't know that it doesn't have enough time for it
 *   - interrupt goes off again
 *   - periodic task misses its next deadline
 *   - event driven task does not run because there is not enough time for it to finish
 *   - periodic task runs
 *  This makes sure that the periodic task is allowed to finish and that the scheduler only runs tasks
 *  when it knows it has enough time to.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

#define TIMER_COUNT 250

RTOS::Event_t e1;

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

ISR(TIMER3_COMPA_vect) {
    static int millis_passed = 0;
    millis_passed++;
    if (millis_passed == 400) {
        TCNT3 = 0x00;       // reset the timer count
        millis_passed = 0;  // reset millis_passed
        RTOS::Event::dispatch(e1);
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
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 0,    "task_delayed", },
        { 1000, "task_periodic", },
        { 2000, "task_periodic", },
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
    task_periodic->period_ms = 1000;
    task_periodic->delay_ms = 1000;
    
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
        if (trace->tag == Mark_Start) {
            static int trace_no = 0;
            trace_no++;
            i64 this_time = RTOS::Time::now();
            switch (trace_no) {
                case 2:
                    assert(trace->mark.start.instance == 0 && this_time >= 400 && this_time < 800);
                    return;
                case 3:
                    assert(trace->mark.start.instance == 0 && this_time >= 800 && this_time < 1000);
                    return;
                case 5:
                    assert(trace->mark.start.instance == 0 && this_time >= 1200 && this_time < 1600);
                    return;
                case 6:
                    assert(trace->mark.start.instance == 0 && this_time >= 1600 && this_time < 2000);
                    return;
                default:
                    break;
            }
        }
        Test::schedule_trace(trace);
    }
    
    bool error(Trace_t * trace) { return true; }
    
}}