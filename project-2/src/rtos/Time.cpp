#include <RTOS.h>

namespace RTOS {
namespace Time {

    i64 now() {
        // TODO time should be managed manually (without Arduino)
        // This should be done using an AVR timer

        // Milliseconds are accurate enough (we want to spend as little time
        // being interrupted as possilbe, more accurate time tracking requires
        // spending time checking the clock.

        // Define configuration values for determining which timer the system
        // should use.

        // We use a signed number so that math transformations of time will 
        // be safe.

        // Use the following for reading value from timer interrupt (to avoid 
        // race conditions)
        //
        // i64 time;
        // ATOMIC_BLOCK(ATOMIC_RESTORSTATE) {
        //     time = isr_time_value;
        // }
        // return time;
        return millis();
    }

    void idle(i64 this_time, i64 idle_time) {

        // Get most accurate idle time
        i64 now_time = now();
        idle_time -= (now_time - this_time);
        
        // Check if time has already passed by now
        if (idle_time < 1) {
            return;
        }

        #ifdef RTOS_TRACE
            Registers::trace.tag = Mark_Idle;
            Registers::trace.mark.idle.time = now();
            trace();
        #endif

        delay(idle_time);
        // TODO proper delay
        // Want to set system to idle
        // Want inerrupts dispatching events to wake us up.
        // RTOS timer should wakeup automatically every ms
        // Something like:
        //
        // while(now() - now_time < idle_time && !Registers.events) {
        //     idle_mode();
        // }

        #ifdef RTOS_TRACE
            RTOS::Registers::trace.tag = Mark_Wake;
            RTOS::Registers::trace.mark.wake.time = now();
            trace();
        #endif
    }

}}