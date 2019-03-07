#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

bool led = false;

bool task_led_fn(RTOS::Task_t * task) {
    led = !led;
    digitalWrite(LED_BUILTIN, led);
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 200,  "task_led", },
        { 700,  "task_led", },
        { 1200, "task_led", },
        { 1700, "task_led", },
        { 0 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    pinMode(LED_BUILTIN, OUTPUT);
    RTOS::Task_t * task_led = RTOS::Task::init("task_led", task_led_fn);
    task_led->period_ms = 500;
    task_led->delay_ms  = 200;
    RTOS::Task::dispatch(task_led);
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