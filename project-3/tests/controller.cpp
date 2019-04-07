#define MESSAGE_SENDER

#include <RTOS.h>
#include "Message.h"
#include "Stick.h"
#include "Peripherals.h"

#define BLUETOOTH_BAUD     9600
#define SERIAL_BUFFER_SIZE 256

// #define SERIAL_MONITOR
// #define PRINT_XY

using namespace RTOS;

static HardwareSerial * bluetooth;
static Stick_t * stick_m;
static Stick_t * stick_u;

bool task_forward_bluetooth_fn(Task_t * task) {
    static char serial_buffer[SERIAL_BUFFER_SIZE + 1];

    if (bluetooth->available()) {
        i16 n = min(bluetooth->available(), SERIAL_BUFFER_SIZE);
        i16 i;
        for (i = 0; i < n; i++) {
            serial_buffer[i] = bluetooth->read();
        }
        serial_buffer[i] = 0;
        debug_print(serial_buffer);
    }
    return true;
}

bool task_sample_fn(Task_t * task) {
    
    Message_t * current_message = (Message_t *) task->state;
    current_message->u_x   = Stick::x(stick_u);
    current_message->u_y   = Stick::y(stick_u);
    current_message->m_x   = Stick::x(stick_m);
    current_message->m_y   = Stick::y(stick_m);
    current_message->flags = Stick::sw(stick_m) ? MESSAGE_LASER : 0;

    #ifdef PRINT_XY
        debug_print(
            "m_x = %d m_y = %d u_x = %d u_y = %d flags = %d\n",
            (int) current_message->m_x,
            (int) current_message->m_y,
            (int) current_message->u_x,
            (int) current_message->u_y,
            (int) current_message->flags
        );
    #endif
    
    // send the message
    u8 * buffer = (u8 *) current_message;
    u16 i;
    for (i = 0; i < sizeof(Message_t); i++) {
        bluetooth->write(buffer[i]);
    }
    
    task->state = (void *) current_message;
    return true;
}

int main() {
    
    RTOS::init();

    #ifdef SERIAL_MONITOR
        Serial.begin(9600);
    #endif

    bluetooth = &Serial1;
    bluetooth->begin(BLUETOOTH_BAUD);
    stick_m   = Stick::init_stick_m();
    stick_u   = Stick::init_stick_u();
    
    Message_t current_message;
    
    // We only set this once
    current_message.header = MESSAGE_HEADER;
    
    Task_t * task_sample = Task::init("task_sample", task_sample_fn);
    task_sample->period_ms = 150; // 32;
    task_sample->delay_ms  = 100;
    task_sample->state = (void *) &current_message;
    Task::dispatch(task_sample);

    Task_t * task_forward_bluetooth = Task::init("task_forward_bluetooth", task_forward_bluetooth_fn);
    task_forward_bluetooth->period_ms = task_sample->period_ms / 5;
    task_forward_bluetooth->delay_ms  = task_sample->delay_ms + task_sample->period_ms / 2;
    Task::dispatch(task_forward_bluetooth);

    RTOS::dispatch();

    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * trace) {
        #ifdef SERIAL_MONITOR
            if (trace->tag == Debug_Message) {
                Serial.print(trace->debug.message);
            }
        #else
            Trace::serial_trace(trace);
        #endif
    }

    bool error(Trace_t * trace) {
        return true;
    }

}}