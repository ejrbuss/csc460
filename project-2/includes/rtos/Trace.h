#pragma once

#ifndef RTOS_TRACE_H
#define RTOS_TRACE_H

namespace RTOS {

    /**
     * The trace tag can be used to decode the data from the Trace struct, 
     * which is implemented as a tagged union for space efficiecny.
     */
    enum Trace_Tag_t {
        // Definitions
        Def_Task,  // The creation of a task
        Def_Event, // The definition of an event
        Def_Alloc, // The allocation of memory
        // Marks
        Mark_Init,  // The start of the RTOS
        Mark_Halt,  // RTOS exucution is about to stop
        Mark_Start, // The start of a task
        Mark_Stop,  // The end of a task
        Mark_Event, // The occurence of an event
        Mark_Idle,  // Scheduled idle time
        Mark_Wake,  // Woke up from idle time
        // Errors
        Error_Max_Event,       // Maximum number of events exceeded
        Error_Undefined_Event, // Undefined event dispatched
        Error_Max_Alloc,       // Maximum memory allocation exceeded
        Error_Max_Pool,        // Maximum pool allocation exceeded
        Error_Null_Pool,       // Null pool or chunk pointer passed as argument
        Error_Max_Task,        // Maximum tasks exceeded
        Error_Null_Task,       // Null task passed as argument
        Error_Invalid_Task,    // Invalid task configuration provided
        Error_Duplicate_Event, // Two tasks act on the same event
        Error_Missed,          // A task schedule was missed
        // Debug
        Debug_Message, // Used to send messages to the tracer
    };

    /**
     * The Trace struct provides detailed information about the ongoing state
     * of the RTOS. There are three types of traees
     * 
     *  1. Definitions 
     *     Relates a handle in the form of a c string with some resource. This
     *     could be the task instance number, the event number, or the amount
     *     of memory allocated.
     * 
     *  2. Marks
     *     Relates an event to a moment in time. This includes the RTOS start,
     *     the scheduling of a task, and the dispatching of events.
     * 
     *  3. Errors
     *     Occur when something unexpected in the system. See the individual
     *     errors for details
     */
    typedef struct Trace_t Trace_t;
    struct Trace_t {
        Trace_Tag_t tag; // The trace tag
        union {
            union {
                struct { const char * handle; };
                struct { const char * handle; u8 instance; } task;
                struct { const char * handle; Event_t event; } event;
                struct { const char * handle; u16 bytes; } alloc;
            } def;
            union {
                struct { u64 time; u16 heap; } init;
                struct { u64 time; } halt;
                struct { u64 time; u8 instance; } start;
                struct { u64 time; u8 instance; } stop;
                struct { u64 time; Event_t event; } event;
                struct { u64 time; } idle;
                struct { u64 time; } wake;
            } mark;
            union {
                struct { Event_t event; } undefined_event;
                struct { Event_t event; } duplicate_event;
                struct { u8 instance; } invalid_task;
                struct { u8 instance; } missed;
            } error;
            union {
                struct { const char * message; };
            } debug;
        };
    };
    
}

#endif /* RTOS_TRACE_H */