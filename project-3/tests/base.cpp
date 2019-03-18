#include <RTOS.h>

using namespace RTOS;

bool tick(Task_t * self) {
    return true;
}

int main() {
    RTOS::init();
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * trace) {
        Trace::serial_trace(trace);
    }

    bool error(Trace_t * trace) {
        return true;
    }

}}