/**
 * schedule_02
 * 
 * Test: 
 *  Multiple periodic tasks that do not overlap run when they are supposed to and succeed.
 */

#include <RTOS.h>
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
    task_led_off->delay_ms = 250;
    RTOS::Trace::configure_pin(task_led_on, 2);
    RTOS::Trace::configure_pin(task_led_off, 3);
    RTOS::Task::dispatch(task_led_on);
    RTOS::Task::dispatch(task_led_off);
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {
    void trace(Trace_t * trace) { 
        Trace::serial_trace(trace);
        Trace::pin_trace(trace);
    }
    bool error(Trace_t * trace) { return true; }
}}