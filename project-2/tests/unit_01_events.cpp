#include <Test.h>
#include <RTOS.h>
#include <Private.h>

int main() {
    RTOS::init();

    using namespace RTOS;
    Event_t e1 = Event::init("event 1");
    Event_t e2 = Event::init("event 2");
    Event_t e3 = Event::init("event 3");
    
    // Test event values
    assert(e1 == 0b001);
    assert(e2 == 0b010);
    assert(e3 == 0b100);
    
    Event::dispatch(e1);
    
    // Test dispatch
    assert(Registers::events & e1);
    assert(!(Registers::events & e2));
    assert(!(Registers::events & e3));

    // Test undefined dispatched
    Event::dispatch(e3 + e3);

    // Test creation of too many events
    int i;
    for (i = 0; i < RTOS_MAX_EVENTS - 2; i++) { 
        Event::init("");
    }
    RTOS::halt();
    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * t) {
        Trace::serial_trace(t);
        static int trace_no = 0;
        if (t->tag != Debug_Message) {
            trace_no++;
            switch (trace_no) {
                // case 1 - 3 RTOS init
                case 4:
                case 5:
                case 6:
                    assert(t->tag == Def_Event);
                    break;
                case 7:
                    assert(t->tag == Mark_Event && t->mark.event.event == 0b1);
                    break;
                default: 
                    break;
            }
        }
    }

    bool error(Trace_t * t) { 
        static int trace_no = 0;
        trace_no++;
        switch (trace_no) {
            case 1:
                assert(t->tag == Error_Undefined_Event);
                break;
            default:
                assert(t->tag == Error_Max_Event);
                break;
        }
        return true; 
    }

}}