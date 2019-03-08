/**
 * schedule_10
 * 
 * Test: 
 *  A delayed task that creates itself is ran twice.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

static bool task_delayed_ran = false;
    
bool task_delayed_fn(RTOS::Task_t * task) {
    static int count = 0;
    if (count == 0) {
        task->delay_ms = 1000;  // delay for 1000ms from when this task was supposed to run
    }
    
    task_delayed_ran = !task_delayed_ran;
    return true;
}

bool task_periodic_fn(RTOS::Task_t * task) {
    static int count = 0;
    count++;
    switch (count) {
        case 1:
            assert(!task_delayed_ran);
            break;
        case 2:
        case 3:
            assert(task_delayed_ran);
            break;            
        default:
            assert(!task_delayed_ran);
            break;
    }
    
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 0,    "task_periodic", },
        { 250,  "task_delayed",  },
        { 500,  "task_periodic", },
        { 1000, "task_periodic", },
        { 1250, "task_delayed",  },
        { 1500, "task_periodic", },
        { 2000, "task_periodic", },
        { -1 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    
    RTOS::Task_t * task_delayed = RTOS::Task::init("task_delayed", task_delayed_fn);
    task_delayed->period_ms = 0;
    task_delayed->delay_ms = 250;
    
    RTOS::Task_t * task_periodic = RTOS::Task::init("task_periodic", task_periodic_fn);
    task_periodic->period_ms = 500;
    
    RTOS::Task::dispatch(task_delayed);
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