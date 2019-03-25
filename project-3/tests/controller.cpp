#define MESSAGE_SENDER

#include <RTOS.h>
#include "Message.h"
#include "Peripherals.h"

#define BAUD 9600

int g_done = 0;

bool task_sample_fn(RTOS::Task_t * task) {
    Message_t * current_message = (Message_t *) task->state; 
    if (photocell_hit()) {
        g_done = 1;
        current_message->flags = MESSAGE_DONE;
    } else {
        current_message->u_x   = sample_stick_u_x();
        current_message->u_y   = sample_stick_u_y();
        // current_message->m_x   = sample_stick_m_x();
        // current_message->m_y   = sample_stick_m_y();
        current_message->flags = stick_u_down() ? MESSAGE_LASER : 0;
    }
    task->state = (void *) current_message;
    return true;
}

bool task_send_message_fn(RTOS::Task_t * task) {
    Message::send((Message_t *) task->state);
    return true;
}

int main() {
    RTOS::init();
    
    Serial1.begin(BAUD);
    Serial.begin(BAUD);
    
    Message_t * current_message = NULL;
    
    // We only set this once
    current_message->header = MESSAGE_HEADER;
    
    RTOS::Task_t * task_sample = RTOS::Task::init("task_sample", task_sample_fn);
    RTOS::Task_t * task_send_message = RTOS::Task::init("task_send_message", task_send_message_fn);
    
    task_sample->period_ms = 16;
    task_sample->state = (void *) current_message;
    
    task_send_message->period_ms = 16;
    task_send_message->delay_ms = 8;
    task_send_message->state = (void *) current_message;
    
    RTOS::Task::dispatch(task_sample);
    RTOS::Task::dispatch(task_send_message);
    RTOS::dispatch();
    
    // while (1) {
    //     if(Serial.available()) {
    //         Serial1.write(Serial.read());
    //     }
    //     if(Serial1.available()) {
    //         Serial.write(Serial1.read());
    //     }
    //     delay(100);
    // }
    
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