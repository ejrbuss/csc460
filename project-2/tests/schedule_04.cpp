/**
 * schedule_04
 * 
 * Test: 
 *  Two periodic tasks that overlap cause an error and the second task is ran ASAP.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

bool task_led_on_fn(RTOS::Task_t * task) {
    digitalWrite(LED_BUILTIN, 0);
    return true;
}

bool task_led_off_fn(RTOS::Task_t * task) {
    digitalWrite(LED_BUILTIN, 1);
    return true;
}

int main() {
    RTOS::init();
    pinMode(LED_BUILTIN, OUTPUT);
    RTOS::Task_t * task_led_on = RTOS::Task::init("task_led_on", task_led_on_fn);
    RTOS::Task_t * task_led_off = RTOS::Task::init("task_led_off", task_led_off_fn);
    task_led_on->period_ms = 500;
    task_led_off->period_ms = 500;
    RTOS::Task::dispatch(task_led_on);
    RTOS::Task::dispatch(task_led_off);
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {
    
    void trace(Trace_t * trace) { 
        Trace::serial_trace(trace);
        static int trace_no = 0;
        if (trace->tag == Mark_Start) {
            trace_no++;
            i64 this_time = RTOS::Time::now();
            switch (trace_no) {
                case 1:
                    assert(trace->tag == Mark_Start && trace->mark.start.instance == 0);
                    assert(this_time == 0);
                    break;
                case 2:
                    assert(trace->tag == Mark_Start && trace->mark.start.instance == 1);
                    assert(this_time >= 0);
                    break;
                default: 
                    break;
            }
        }
    }
    
    bool error(Trace_t * trace) { 
        static int trace_no = 0;
        trace_no++;
        switch (trace_no) {
            case 1:
                assert(trace->tag == Error_Missed);
                RTOS::debug_print("\nTest passed.\n");
                RTOS::halt();
                break;
            default:
                break;
        }
        return true;
    }
    
}}