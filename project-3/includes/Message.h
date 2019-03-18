#pragma once

#ifndef MESSAGE_H
#define MESSAGE_H

#include <RTOS.h>

typedef struct Message_t Message_t;
struct Message_t {
#ifdef MESSAGE_SENDER
    u32 header;
#endif
    s8 u_x;
    s8 u_y;
    s8 m_x;
    s8 m_y;
    u8 flags;
};

namespace Message {

    Message * init();

    const char * to_string(Message_t * message);

    Message * from_byte_stream(u8 byte);

    void done(Message_t * message);
    void done(Message_t * message, bool done);
    void stick_u_down(Message_t * message);
    void stick_u_down(Message_t * message, bool stick_u_down);
    void stick_m_down(Message_t * message);
    void stick_m_down(Message_t * message, bool stick_m_down);

}

#endif /* MESSAGE_H */