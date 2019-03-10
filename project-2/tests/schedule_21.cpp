/**
 * schedule_21
 * 
 * Test:
 *  A task's state stays the same when it is switched between task types.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

bool task_periodic_fn(RTOS::Task_t * task) {
    static int count = 0;
    count++;
    int state = (int) task->state;
    if (count == 1) {
        assert(state == false);
    } else if (count == 2) {
        task->period_ms = 0;
        task->delay_ms = 250;
        assert(state == false);
        task->state = (void *) true;
    } else {
        assert(state == true);
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
    task_periodic->state = (void *) false;
    
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