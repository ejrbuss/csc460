/**
 * schedule_09
 * 
 * Test: 
 *  An immediate task (delay_ms = 0, period_ms = 0) that terminates is ran once and then never again.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

static bool task_delayed_ran = false;
    
bool task_delayed_fn(RTOS::Task_t * task) {
    assert(!task_delayed_ran);
    task_delayed_ran = !task_delayed_ran;
    return true;
}

bool task_periodic_fn(RTOS::Task_t * task) {
    assert(task_delayed_ran);
    return true;
}

int main() {
    Test::Schedule_t schedule[] = {
        { 0,    "task_delayed",  },
        { 500,  "task_periodic", },
        { 1000, "task_periodic", },
        { 1500, "task_periodic", },
        { -1 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    
    RTOS::Task_t * task_delayed = RTOS::Task::init("task_delayed", task_delayed_fn);
    task_delayed->period_ms = 0;
    task_delayed->delay_ms = 0;
    
    RTOS::Task_t * task_periodic = RTOS::Task::init("task_periodic", task_periodic_fn);
    task_periodic->period_ms = 500;
    task_periodic->delay_ms = 500;
    
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