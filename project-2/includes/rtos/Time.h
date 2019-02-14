#pragma once

#ifndef RTOS_TIME
#define RTOS_TIME

namespace RTOS::Time {

        // TODO manage current time manually
        // Could reimplmeent millis using a timer
        // milliseconds are probably as acccurate as we want to be
        // more accurate time requires more time spent updating the time
        // some fuzz helps garuntee a correct schedule is maintained

        // Will probably have to add a configuration macro for what timer
        // should bee used. Chances are this implementation should be moved
        // along with idle() to a Time.cpp file.

        // Eventually change to be a global value (register) ie.
        // s64 time = Registers::now;

        // We use a signed number so that math transformations of time will 
        // be safe

        // Could use micros() but then need to come up with a nice way to 
        // handle overflow

    /**
     * Returns the current time in ms.
     * 
     * @returns i64 the current time
     */
    i64 now();

    /**
     * Idles for `time` from `from`. If time has already passed the idle time
     * will be reduced accordingly. Produces an idle trace.
     * 
     * @param i64 from the time to idle from
     * @param i64 time the amount of time to idle for
     */
    void idle(i64 from, i64 time);

}

#endif