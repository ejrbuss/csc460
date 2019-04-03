#define MESSAGE_SENDER

#include <RTOS.h>
#include "Message.h"
#include "Peripherals.h"

#define SERIAL_BAUD 9600

using namespace RTOS;

bool task_sample_fn(RTOS::Task_t * task) {
    // while (Serial1.available()) {
    //     debug_print("%c", Serial1.read());
    //}
    
    Message_t * current_message = (Message_t *) task->state;
    current_message->u_x   = sample_stick_u_x();
    current_message->u_y   = sample_stick_u_y();
    current_message->m_x   = sample_stick_m_x();
    current_message->m_y   = sample_stick_m_y();
    current_message->flags = stick_u_down() ? MESSAGE_LASER : 0;

    RTOS::debug_print("u_x: %d u_y: %d\n", (int) current_message->u_x, (int) current_message->u_y);
    RTOS::debug_print("m_x: %d m_y: %d\n", (int) current_message->m_x, (int) current_message->m_y);
    
    // send the message
    u8 * buffer = (u8 *) current_message;
    u16 i;
    for (i = 0; i < sizeof(Message_t); i++) {
        Serial1.write(buffer[i]);
    }
    
    task->state = (void *) current_message;
    return true;
}

int main() {
    RTOS::init();
    init_stick_u_sw();
    
    Serial1.begin(SERIAL_BAUD);
    delay(100);
    
    Message_t current_message;
    
    // We only set this once
    current_message.header = MESSAGE_HEADER;
    
    RTOS::Task_t * task_sample = RTOS::Task::init("task_sample", task_sample_fn);
    
    task_sample->period_ms = 500; // v80; //32;
    task_sample->state = (void *) &current_message;
    
    delay(1000);
    RTOS::debug_print("%d\n", Q78_to_int(STICK_M_MAX_X));
    delay(1000);
    
    RTOS::Task::dispatch(task_sample);
    RTOS::dispatch();

    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * trace) {
        Trace::serial_trace(trace);
        if (trace->tag == Debug_Message) {
            // Serial.print(trace->debug.message);
        }
        return;
    }

    bool error(Trace_t * trace) {
        return true;
    }

}}