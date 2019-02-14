#include <RTOS.h>

namespace RTOS {

    namespace Registers {
        Event_t events;
        Trace_t trace;
    }

    void halt() {
        // TODO
    }

    u64 now() {
        return 0;
    }

    void trace() {
        UDF::trace(&Registers::trace);
    }

    void error() {
        UDF::trace(&Registers::trace);
        if (!UDF::error(&Registers::trace)) {
            halt();
        }
    }

}