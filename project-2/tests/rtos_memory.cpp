#include <Test.h>
#include <RTOS.h>

int led = 13;

int main() {
    Test::init(); 

    using namespace RTOS;

    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * t) {
        static int trace_no = 0;
        trace_no++;
        switch (trace_no) {
            default: 
                break;
        }
    }

    bool error(Trace_t * t) { 
        static int trace_no = 0;
        trace_no++;
        switch (trace_no) {
            default:
                break;
        }
        return true; 
    }

}}