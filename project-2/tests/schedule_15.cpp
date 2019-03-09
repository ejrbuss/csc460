/**
 * schedule_15
 * 
 * Test:
 *  A periodic task can become a delayed task for one time and then terminate.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

bool task_periodic_fn(RTOS::Task_t * task) {
    static int count = 0;
    count++;
    if (count == 2) {
        task->period_ms = 0;
        task->delay_ms = 250;
    }
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 0,   "task_periodic", },
        { 500, "task_periodic", },
        { 750, "task_periodic", },
        { -1 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    
    RTOS::Task_t * task_periodic = RTOS::Task::init("task_periodic", task_periodic_fn);
    task_periodic->period_ms = 500;
    
    RTOS::Task::dispatch(task_periodic);
    
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