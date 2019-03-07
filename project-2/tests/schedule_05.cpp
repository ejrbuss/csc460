#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

bool task_one_fn(RTOS::Task_t * task) {
    return true;
}

bool task_two_fn(RTOS::Task_t * task) {
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 0,    "task_led_on" },
        { 0,    "task_led_off" },
        { 500,  "task_led_on" },
        { 500,  "task_led_off" },
        { 1000, "task_led_on" },
        { 1000, "task_led_off" },
        { -1 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    pinMode(LED_BUILTIN, OUTPUT);
    RTOS::Task_t * task_one = RTOS::Task::init("task_one", task_one_fn);
    RTOS::Task_t * task_two = RTOS::Task::init("task_two", task_two_fn);
    task_one->period_ms = 500;
    task_two->period_ms = 500;
    RTOS::Task::dispatch(task_one);
    RTOS::Task::dispatch(task_two);
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