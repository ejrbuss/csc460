#include <RTOS.h>
#include <Private.h>

namespace RTOS::Task {

    static u8 instance_count = 0;
    static Event_t taken_events = 0;

    Task_t * init(const char * handle, task_fn_t fn) {

        Task_t * task = (Task_t *) Memory::Pool::alloc(Registers::task_pool);
        task->fn            = fn;
        task->state         = nullptr;
        task->events        = 0;
        task->period_ms     = 0;
        task->delay_ms      = 0;
        task->impl.last     = 0;
        task->impl.maximum  = 0;
        task->impl.instance = instance_count++;

        #ifdef RTOS_TRACE
            Registers::trace.tag = Def_Task;
            Registers::trace.def.task.handle = handle;
            Registers::trace.def.task.instance = task->impl.instance;
        #endif

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
        if (fn == nullptr) {
            Registers::trace.tag = Error_Invalid_Task;
            Registers::trace.error.invalid_task.instance = task->impl.instance;
            error();
        }
        if (instance_count > RTOS_MAX_TASKS) {
            Registers::trace.tag = Error_Max_Task;
            error();
        }
        #endif

        return task;
    }

    void dispatch(Task_t * task) {

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
        if (task == nullptr) {
            Registers::trace.tag = Error_Null_Task;
            error();
        }
        if (
            (task->events && task->period_ms) ||
            (task->events && task->delay_ms)
        ) {
            Registers::trace.tag = Error_Invalid_Task;
            Registers::trace.error.invalid_task.instance = task->impl.instance;
            error();
        }
        if (task->events & taken_events) {
            Registers::trace.tag = Error_Duplicate_Event;
            Registers::trace.error.duplicate_event.event = task->events & taken_events;
            error();
        }
        taken_events |= task->events;
        #endif

        if (task->period_ms) {
            Registers::periodic_tasks = Task::insert_ordered(Registers::periodic_tasks, task);
        } else if (task->delay_ms) {
            Registers::delayed_tasks = Task::insert_ordered(Registers::delayed_tasks, task);
        } else {
            Registers::event_tasks_tail = Task::insert_tail(Registers::event_tasks_tail, task);
        }
    }

    void run(Task_t * task) {

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
        if (task == nullptr) {
            Registers::trace.tag = Error_Null_Task;
            error();
        }
        #endif

        Event_t save = task->events;
        Registers::triggers |= (Registers::events & save);

        // Atomically update events
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            Registers::events = (Registers::events & ~save);
        }

        i64 now = Task::time_next(task);

        // Check for miss
        if (Time::now() > now) {
            Registers::trace.tag = Error_Missed;
            Registers::trace.error.missed.instance = task->impl.instance;
            error();
        }

        // Reset delay
        task->delay_ms = 0;

        #ifdef RTOS_TRACE
            Registers::trace.tag = Mark_Start;
            Registers::trace.mark.start.time = Time::now();
            Registers::trace.mark.start.instance = task->impl.instance;
            trace();
        #endif

        // Run task
        bool result = task->fn(task);

        #ifdef RTOS_TRACE
            Registers::trace.tag = Mark_Stop;
            Registers::trace.mark.stop.time = Time::now();
            Registers::trace.mark.stop.instance = task->impl.instance;
            trace();
        #endif

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
        if (
            (task->events && task->period_ms) ||
            (task->events && task->delay_ms)
        ) {
            Registers::trace.tag = Error_Invalid_Task;
            Registers::trace.error.invalid_task.instance = task->impl.instance;
            error();
        }
        #endif

        // Update fields
        task->impl.last = now;
        task->impl.maximum = max(task->impl.maximum, Time::now() - now);

        if (save) {
            if (!task->events || !result) {

                #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
                taken_events &= ~task->events;
                #endif

                // Remove task from event_tasks
                if (Registers::event_tasks == task) {
                    Registers::event_tasks = Task::cdr(task);
                } else {
                    Task_t * event_tasks = Registers::event_tasks;
                    while (event_tasks) {
                        if (Task::cdr(event_tasks) == task) {
                            Memory::Pool::cons(event_tasks, Task::cdr(event_tasks));
                        }
                        event_tasks = Task::cdr(event_tasks);
                    }
                }
            }

        }

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
        if (task->events & taken_events) {
            Registers::trace.tag = Error_Duplicate_Event;
            Registers::trace.error.duplicate_event.event = task->events & taken_events;
            error();
        }
        taken_events |= task->events;
        #endif

        if (!result) {
            Memory::Pool::dealloc(Registers::task_pool, task);                
        } else if (task->period_ms) {
            Registers::periodic_tasks = Task::insert_ordered(Registers::periodic_tasks, task);
        } else if (task->delay_ms) {
            Registers::delayed_tasks = Task::insert_ordered(Registers::delayed_tasks, task);
        } else if (task->events) {
            Registers::event_tasks_tail = Task::insert_tail(Registers::event_tasks_tail, task);
        } else {
            Memory::Pool::dealloc(Registers::task_pool, task);
        }
    }

    bool fits(Task_t * task, i64 time) {

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
        if (task == nullptr) {
            Registers::trace.tag = Error_Null_Task;
            error();
        }
        #endif

        return task->impl.maximum < time;
    }

    Task_t * cdr(Task_t * tasks) {
        return (Task_t *) Memory::Pool::cdr(tasks);
    }

    Task_t * insert_tail(Task_t * tasks_tail, Task_t * task) {
        Memory::Pool::cons(tasks_tail, task);
        return task;
    }

    Task_t * insert_ordered(Task_t * tasks, Task_t * task) {

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
        if (task == nullptr) {
            Registers::trace.tag = Error_Null_Task;
            error();
        }
        #endif

        i64 time_next = Task::time_next(task);

        if (tasks == nullptr || Task::time_next(tasks) > time_next) {
            Memory::Pool::cons(task, tasks);
            return tasks;
        } else {
            for (;;) {
                Task_t * cdr = Task::cdr(tasks);
                if (cdr == nullptr || Task::time_next(cdr) > time_next) {
                    Memory::Pool::cons(tasks, task);
                    break;
                }
                tasks = cdr;
            }
        }
        return tasks;
    }

    i64 time_next(Task_t * task) {

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
        if (task == nullptr) {
            Registers::trace.tag = Error_Null_Task;
            error();
        }
        #endif

        return task->impl.last + task->period_ms + task->delay_ms;
    }

    i64 time_remaining(Task_t * task, i64 time_ms) {

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_TASK)
        if (task == nullptr) {
            Registers::trace.tag = Error_Null_Task;
            error();
        }
        #endif

        return Task::time_next(task) - time_ms;
    }

}