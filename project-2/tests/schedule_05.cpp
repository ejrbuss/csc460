/**
 * schedule_05
 * 
 * Test:
 * Test the ordering of delayed tasks.
 *   - Create one delayed task to run at 500
 *   - Create one delayed task to run at 300
 *   - Start the OS
 *   - Check that they ran in the correct order.
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
    Test::Schedule_t schedule[] = {
        { 0,    "task_led_on" },
        { 250,  "task_led_off" },
        { 500,  "task_led_on" },
        { 750,  "task_led_off" },
        { 1000, "task_led_on" },
        { 1250, "task_led_off" },
        { 1500, "task_led_on" },
        { 1750, "task_led_off" },
        { 2000, "task_led_on" },
        { -1 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    pinMode(LED_BUILTIN, OUTPUT);
    RTOS::Task_t * task_led_off = RTOS::Task::init("task_led_off", task_led_off_fn);
    RTOS::Task_t * task_led_on = RTOS::Task::init("task_led_on", task_led_on_fn);
    task_led_off->period_ms = 500;
    task_led_off->delay_ms = 250;
    task_led_on->period_ms = 500;
    RTOS::Task::dispatch(task_led_off);
    RTOS::Task::dispatch(task_led_on);
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {
    void trace(Trace_t * trace) { 
        Trace::serial_trace(trace);
        Test::schedule_trace(trace);
    }
    bool error(Trace_t * trace) { return true; }
}}