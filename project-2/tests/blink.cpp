#include "Base.h"
#include "Peripherals.h"
#include <RTOS.h>

bool led = false;

bool task_led_fn(RTOS::Task_t * task) {
    led = !led;
    digitalWrite(LED_BUILTIN, led);
    return true;
}

int main() {
    init_arduino();
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(SERIAL_BAUD);
    Serial.write(sizeof(RTOS::Event_t));
    RTOS::init();
    RTOS::Task_t * task_led = RTOS::Task::init("task_led", task_led_fn);
    task_led->period_ms = 500;
    RTOS::Task::dispatch(task_led);
    RTOS::dispatch();
    return 0;
}

namespace RTOS::UDF {

    void trace(Trace_t * trace) {
        u8 * trace_buffer = (u8 *) trace;
        for (u16 i = 0; i < sizeof(Trace_t); i++) {
            Serial.write(trace_buffer[i]);
        }
        if (trace->tag < Mark_Init) {
            Serial.print(trace->def.handle);
            Serial.write('\0');
        }
    }
    bool error(Trace_t * trace) { return true; }

}