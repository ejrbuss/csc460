#include <RTOS.h>

using namespace RTOS;

int main() {
    RTOS::init();
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * trace) {
        return;
    }

    bool error(Trace_t * trace) {
        return true;
    }

}}