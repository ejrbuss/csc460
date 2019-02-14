#pragma once

#ifndef RTOS_H
#define RTOS_H

//
// Macros
//

// Macros for generating numeric types
#define NUMTYPE_U_IMPL(n) u ## n
#define NUMTYPE_U(n) NUMTYPE_U_IMPL(n)
#define NUMTYPE_I_IMPL(n) i ## n
#define NUMTYPE_I(n) NUMTYPE_i_IMPL(n)
#define BV(bit) (1 << (bit))

//
// Exact numeric types
//

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned long      u32;
typedef unsigned long long u64;

typedef signed char      i8;
typedef signed int       i16;
typedef signed long      i32;
typedef signed long long i64;

#include "Conf.h"
#include "CheckConf.h"
#include "Event.h"
#include "Task.h"
#include "Trace.h"
#include "Memory.h"

namespace RTOS {

    /**
     * Starts the RTOS.
     */
    void init();

    /**
     * Immediately halts the execution of the RTOS, producing a trace error.
     */
    void halt();

        /**
     * Traces the current trace register
     */
    void trace();

    /**
     * Traces the current trace register sending it to both UDF::trace and 
     * UDF::error.
     */
    void error();

    /**
     * Returns the current time in milliseconds.
     */
    u64 now();

    /**
     * Registers are bits of data the RTOS needs to share across its entire
     * system and may want to share publically.
     */
    namespace Registers {

        // A value that will be set with the bitwise ORed values of all active
        // events
        extern Event_t events;

        // The last trace that occured.
        extern Trace_t trace;

    }

    /**
     * User defined functions, these must be implemented by YOU!
     */
    namespace UDF {

        /**
         * Create initial tasks, setup hardware, etc. Once this function is 
         * returned from the scheduler will start running.
         * 
         * eg.
         *   #include <RTOS.h>
         * 
         *   int main() { 
         *      RTOS::init();
         *      return 0;
         *   }
         * 
         *   namespace RTOS::UDF {
         * 
         *       bool task_blink_LED_fn(Task_t * self) {
         *           bool * on = (bool *) self->state;
         *           *on = !(*on);
         *           digitalWrite(BUILTIN_LED, *on);
         *           return true; // Schedule me agin!
         *       }
         * 
         *       void init() {
         *           // Initialize LED
         *           pinMode(BUILTIN_LED, Output);
         *           // Create a task
         *           Task_t * task_blink_LED = Task::from_function(
         *               "task_blink_LED",  // A debug handle for tracing
         *               task_blink_LED_fn, // The task function
         *           );
         *           // Initialize state
         *           bool * state = Memory::static_alloc("LED state", sizeof(bool));
         *           *state = false;
         *           task_blink_LED->state = state;  // Set state
         *           task_blink_LED->period = 1000;  // Set period (in milliseconds)
         *           Task::dispatch(task_blink_LED); // Dispatch the task
         *       }
         *   }
         */
        void init();
        
        /**
         * Handles tracing data. Traces are provided for a variety actions that
         * occur in the RTOS (see Trace.h for more information). This function
         * is only called if RTOS_TRACE is defined. 
         * 
         * RTOS provides two builtin methods of handling trace data:
         * 
         *  1. PIN MODE
         *     In PIN MODE RTOS will set various available digital pins high
         *     and low to indicate when tasks are running.
         *  2. SERIAL MODE
         *     In SERIAL MODE RTOS will send all trace informatin out on a 
         *     serial port as it comes in.
         * 
         * eg.
         * 
         *   void RTOS::UDF::init() {
         *       use RTOS;
         *       ...
         *       // Set the pin for a given task
         *       Trace::PinTrace(my_task, PIN_12);
         *   }
         *  
         *   void RTOS::UDF::trace(const Trace_t * trace) {
         *       // Tell PIN MODE to handle trace
         *       RTOS::Trace::PinMode(trace);
         *   }
         * 
         * See Trace.h for how to use these builtin modes
         * 
         * @param const Trace_t * trace the trace action that just occured
         */
        void trace(Trace_t * trace);

        /**
         * This function is called with an error trace regardless if RTOS_TRACE
         * is defined. This function provides you a chance to handle errors 
         * gracefully. Returning true from this function will resume scheduling.
         * Returning false from this function will halt scheduling.
         */
        bool error(Trace_t * trace);

    }
    
}

#endif /* RTOS_H */