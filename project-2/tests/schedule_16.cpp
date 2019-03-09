/**
 * schedule_16
 * 
 * Test:
 *  A delayed task can become a periodic task.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

bool task_delayed_fn(RTOS::Task_t * task) {
    task->period_ms = 500;
    task->delay_ms = 0;
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 250,  "task_delayed", },
        { 750,  "task_delayed", },
        { 1250, "task_delayed", },
        { -1 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    
    RTOS::Task_t * task_delayed = RTOS::Task::init("task_delayed", task_delayed_fn);
    task_delayed->delay_ms = 250;
    
    RTOS::Task::dispatch(task_delayed);
    
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