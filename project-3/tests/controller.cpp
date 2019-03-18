#include <RTOS.h>

#define BAUD 9600

using namespace RTOS;

bool tick(Task_t * self) {
    return true;
}

int main() {
    RTOS::init();
    // RTOS::dispatch();
    
    Serial1.begin(BAUD);
    Serial.begin(BAUD);
    Serial.println("Starting init");
    delay(1000);
    
    while (1) {
        if(Serial.available()) {
            Serial1.write(Serial.read());
        }
        if(Serial1.available()) {
            Serial.write(Serial1.read());
        }
        delay(100);
    }
    
    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * trace) {
        // Trace::serial_trace(trace);
        return;
    }

    bool error(Trace_t * trace) {
        return true;
    }

}}