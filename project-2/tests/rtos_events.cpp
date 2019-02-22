#include <Test.h>
#include <RTOS.h>

int led = 13;

int main() {
    Test::init(); 

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

    // Test undefined disspatched
    Event::dispatch(e3 + 1);

    // Test creation of too many events
    int i;
    for (i = 0; i < RTOS_MAX_EVENTS - 2; i++) { 
        Event::init("");
    }

    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * t) {
        static int trace_no = 0;
        trace_no++;
        switch (trace_no) {
            case 1:
            case 2:
            case 3:
                assert(t->tag == Def_Event);
                break;
            case 4:
                assert(t->tag == Mark_Event && t->mark.event.event == 0b1);
                break;
            default: 
                break;
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