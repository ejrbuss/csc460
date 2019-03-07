#include <RTOS.h>
#include <Test.h>
#include "Peripherals.h"

#define TIMER_COUNT 100

bool led = false;

bool task_led_fn(RTOS::Task_t * task) {
    led = !led;
    digitalWrite(LED_BUILTIN, led);
    for (i8 i = 0; i < 1000; i+=) {}    // busy wait
    return true;
}

ISR(TIMER2_COMPA_vect) {
    RTOS::Event::dispatch();
    return;
}

void timer_init() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCCR2A = 0x00;                 // Clear control register A
        TCCR2B = 0x00;                 // Clear control register B
        TCNT2  = 0x00;                 // Clear the counter
        OCR2A  = TIMER_COUNT;          // The value we are waiting for
        TCCR2B |= BV(WGM22);           // Use CTC mode
        TCCR2B |= BV(CS21) | BV(CS20); // Scale by 64
        TIMSK2 |= BV(OCIE2A);          // Enable timer compare interrupt
    }
}

int main() {
    Test::Schedule_t schedule[] = {
        { 0,    "task_led", },
        { 500,  "task_led", },
        { 1000, "task_led", },
        { 1500, "task_led", },
        { 2000, "task_led", },
        { -1 },
    };
    Test::set_schedule(schedule);
    RTOS::init();
    pinMode(LED_BUILTIN, OUTPUT);
    RTOS::Task_t * task_led = RTOS::Task::init("task_led", task_led_fn);
    task_led->period_ms = 500;
    RTOS::Task::dispatch(task_led);
    timer_init();
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {
    void trace(Trace_t * trace) {
        Trace::serial_trace(trace);
        Test::schedule_trace(trace);
    }
    bool error(Trace_t * trace) { return true; }
}}