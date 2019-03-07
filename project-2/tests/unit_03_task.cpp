#include <RTOS.h>
#include <Test.h>
#include <Private.h>

int led = 13;

bool task_fn(RTOS::Task_t * _) {
    return true;
}

int main() {
    
    using namespace RTOS;

    RTOS::init();

    // Test periodic task ordering
    Task_t * t1 = Task::init("t1", task_fn);
    t1->period_ms = 100;
    t1->delay_ms  = 100;
    Task::dispatch(t1);

    Task_t * t2 = Task::init("t2", task_fn);
    t2->period_ms = 100;
    t2->delay_ms  = 100;
    Task::dispatch(t2);

    Task_t * t3 = Task::init("t3", task_fn);
    t3->period_ms = 100;
    Task::dispatch(t3);

    assert(Registers::periodic_tasks == t3);
    assert(Task::cdr(Registers::periodic_tasks) == t1);
    assert(Task::cdr(Task::cdr(Registers::periodic_tasks)) == t2);
    assert(Task::cdr(Task::cdr(Task::cdr(Registers::periodic_tasks))) == nullptr);
    
    // Test delayed task ordering
    Task_t * t4 = Task::init("t4", task_fn);
    t4->delay_ms = 100;
    Task::dispatch(t4);

    Task_t * t5 = Task::init("t5", task_fn);
    t5->delay_ms = 100;
    Task::dispatch(t5);
    
    Task_t * t6 = Task::init("t6", task_fn);
    t6->delay_ms = 1;
    Task::dispatch(t6);

    assert(Registers::delayed_tasks == t6);
    assert(Task::cdr(Registers::delayed_tasks) == t4);
    assert(Task::cdr(Task::cdr(Registers::delayed_tasks)) == t5);
    assert(Task::cdr(Task::cdr(Task::cdr(Registers::delayed_tasks))) == nullptr);

    Event_t e1 = Event::init("e1");
    Event_t e2 = Event::init("e2");

    // Test Event task ordering
    Task_t * t7 = Task::init("t7", task_fn);
    t7->events = e1;
    Task::dispatch(t7);

    Task_t * t8 = Task::init("t8", task_fn);
    t8->events = e2;
    Task::dispatch(t8);

    assert(Registers::event_tasks == t7);
    assert(Task::cdr(Registers::event_tasks) == t8);
    assert(Task::cdr(Task::cdr(Registers::event_tasks)) == nullptr);

    // Test that you cannot have two tasks with the same event
    Task_t * t9 = Task::init("t9", task_fn);
    t9->events = e2;
    Task::dispatch(t9);

    // Test invalid task
    Task_t * t10 = Task::init("t10", task_fn);
    Task::dispatch(t10);

    // Test too many tasks
    int i;
    for (i = 10; i <= RTOS_MAX_TASKS; i++) {
        Task::init("handle", task_fn);
    }

    RTOS::halt();
    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * t) {
        Trace::serial_trace(t);
    }

    bool error(Trace_t * t) { 
        static int trace_no = 0;
        trace_no++;
        switch (trace_no) {
            case 1:
                assert(t->tag == Error_Duplicate_Event);
                break;
            case 2:
                assert(t->tag == Error_Invalid_Task);
                break;
            case 3:
                assert(t->tag == Error_Max_Pool);
                break;
            case 4:
                assert(t->tag == Error_Max_Task);
                break;
            default:
                break;
        }
        return true; 
    }

}}