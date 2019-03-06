#include <RTOS.h>
#include <Private.h>

namespace RTOS {

    namespace Registers {

        // Public registers
        Event_t triggers;
        Event_t events;
        Trace_t trace;

        // Private registers        
        Memory::Pool_t * task_pool;
        Task_t * periodic_tasks;
        Task_t * delayed_tasks;
        Task_t * event_tasks;
        Task_t * event_tasks_tail;

    }

    void dispatch() {
        for (;;) {
 
            i64 this_time = Time::now();
            i64 idle_time = 0xFFFF;

            Task_t * task;

            task = Registers::periodic_tasks;
            if (task != nullptr) {
                i64 time_remaining = Task::time_remaining(task, this_time);
                if (time_remaining <= 0) {
                    // We need to pop the task off the list before running the task
                    // Run will handle re-inserting it correctly
                    Registers::periodic_tasks = Task::cdr(task);
                    Task::run(task);
                    continue;
                } else {
                    idle_time = min(idle_time, time_remaining);
                }
            }

            task = Registers::delayed_tasks;
            if (task != nullptr) {
                i64 time_remaining = Task::time_remaining(task, this_time);
                if (time_remaining <= 0) {
                    if (Task::fits(task, idle_time)) {
                        // We need to pop the task off the list before running 
                        // the task. Run will handle re-inserting it correctly
                        Registers::delayed_tasks = Task::cdr(task);
                        Task::run(task);
                    }   
                    continue;
                } else {
                    idle_time = min(idle_time, time_remaining);
                }
            }

            task = Registers::event_tasks;
            if (Registers::events) {
                while (task != nullptr) {
                    if (task->events & Registers::events) {
                        if (Task::fits(task, idle_time)) {
                            Task::run(task);
                        }
                        continue;
                    }
                    task = Task::cdr(task);
                }
            }
            Time::idle(this_time, idle_time);
        }
    }

    void init() {

        #ifdef RTOS_USE_ARDUINO
            init_arduino();
        #endif

        #ifdef RTOS_TRACE
            Registers::trace.tag = Mark_Init;
            Registers::trace.mark.init.time = Time::now();
            Registers::trace.mark.init.heap = RTOS_VIRTUAL_HEAP;
            trace();
        #endif

        Registers::task_pool = Memory::Pool::init(
            "RTOS::Registers::task_pool", 
            sizeof(Task_t), 
            RTOS_MAX_TASKS
        );
    }

    void halt() {

        #ifdef RTOS_TRACE
            Registers::trace.tag = Mark_Halt;
            Registers::trace.mark.halt.time = Time::now();
            trace();
        #endif 

        exit(0);
    }

    void trace() {
        #ifdef RTOS_TRACE
        UDF::trace(&Registers::trace);
        #endif
    }

    void debug_print(const char * fmt, ...) {
        #ifdef RTOS_TRACE
            static char buffer[RTOS_MESSAGE_BUFFER];
            va_list args;
            va_start(args, fmt);
            vsnprintf(buffer, RTOS_MESSAGE_BUFFER, fmt, args);
            RTOS::Registers::trace.tag = Debug_Message;
            RTOS::Registers::trace.debug.message = buffer;
            trace();
            va_end(args);
        #endif
    }

    void error() {
        trace();
        if (!UDF::error(&Registers::trace)) {
            halt();
        }
    }

}