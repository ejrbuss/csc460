#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

bool task_led_fn(RTOS::Task_t * task) {
    bool led = task->state;
    digitalWrite(LED_BUILTIN, led);
    if (task->impl.instance == 0) {
        assert((int) task->state == false);
    } else if (task->impl.instance == 1) {
        assert((int) task->state == true);
    }
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 0,    "task_led", 0},
        { 500,  "task_led", 1},
        { 1000, "task_led", 0},
        { 1500, "task_led", 1},
        { 2000, "task_led", 0},
        { -1 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    pinMode(LED_BUILTIN, OUTPUT);
    RTOS::Task_t * task_led_1 = RTOS::Task::init("task_led_1", task_led_fn);
    RTOS::Task_t * task_led_2 = RTOS::Task::init("task_led_2", task_led_fn);
    task_led_1->period_ms = 500;
    task_led_1->state = (void *) false;
    task_led_2->period_ms = 500;
    task_led_2->delay_ms = 250;
    task_led_2->state = (void *) true;
    RTOS::Task::dispatch(task_led_1);
    RTOS::Task::dispatch(task_led_2);
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