/**
 * schedule_07
 * 
 * Test:
 *  An event occurring before the OS is dispatched should be remembered and handled correctly.
 */

#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

bool task_event_1_fn(RTOS::Task_t * task) {
    return true;
}

int main() {
    RTOS::init();

    RTOS::Event_t e1 = RTOS::Event::init("event 1");
    RTOS::Event::dispatch(e1);

    RTOS::Task_t * task_event_1 = RTOS::Task::init("task_event_1", task_event_1_fn);
    task_event_1->events |= e1;
    
    RTOS::Task::dispatch(task_event_1);
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {
    
    void trace(Trace_t * trace) { 
        Trace::serial_trace(trace);
        static int trace_no = 0;
        if (trace->tag != Debug_Message) {
            trace_no++;
            switch (trace_no) {
                // case 1 - 3 RTOS init
                // case 4 Def Event
                case 5:
                    assert(trace->tag == Mark_Event && trace->mark.event.event == 0b1);
                    break;
                // case 6 Def Task
                case 7:
                    assert(trace->tag == Mark_Start);
                    RTOS::debug_print("\nTest passed.\n");
                    RTOS::halt();
                    break;
                default: 
                    break;
            }
        }
    }
    
    bool error(Trace_t * trace) { return true; }
    
}}