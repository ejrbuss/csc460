#include <RTOS.h>

namespace RTOS::Event {

    static u8 event_count = 0;

    Event_t init(const char * handle) {

        Event_t event = BV(event_count);
        event_count++;

        #ifdef RTOS_TRACE
        Registers::trace.tag = Def_Event;
        Registers::trace.def.event.handle = handle;
        Registers::trace.def.event.event = event;
        trace();
        #endif

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_EVENT)
        if (event_count > RTOS_MAX_EVENTS) {
            Registers::trace.tag = Error_Max_Event;
            error();
        }
        #endif

        return event;
    }

    void dispatch(Event_t e) {

        Registers::events |= e;

        #ifdef RTOS_TRACE
        Registers::trace.tag = Mark_Event;
        Registers::trace.mark.event.time = now();
        Registers::trace.mark.event.event = e;
        trace();
        #endif

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_EVENT)
        if (e > (Event_t) BV(event_count)) {
            Registers::trace.tag = Error_Undefined_Event;
            error();
        }
        #endif
    }

}