#pragma once

#ifndef MESSAGE_H
#define MESSAGE_H

#include <RTOS.h>

#define MESSAGE_HEADER ((u32) 0x04030201)
#define MESSAGE_DONE   0b00000001
#define MESSAGE_LASER  0b00000010

typedef struct Message_t Message_t;
struct Message_t {
#ifdef MESSAGE_SENDER
    u32 header;
#endif
    i8 u_x;
    i8 u_y;
    i8 m_x;
    i8 m_y;
    u8 flags;
};

namespace Message {

    Message_t * init();

    Message_t * receive(Message_t * current_message);
    
    void print(Print & p, const Message_t * m);

    const char * to_string(Message_t * message);

    Message_t * from_byte_stream(u8 byte);

    void done(Message_t * message);
    void done(Message_t * message, bool done);
    void stick_u_down(Message_t * message);
    void stick_u_down(Message_t * message, bool stick_u_down);
    void stick_m_down(Message_t * message);
    void stick_m_down(Message_t * message, bool stick_m_down);

}

#endif /* MESSAGE_H */