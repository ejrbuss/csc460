#pragma once

#ifndef PHASE_2_H
#define PHASE_2_H

#define MESSAGE_HEADER ((u32) 0x04030201)
#define MESSAGE_DONE   0b00000001
#define MESSAGE_LASER  0b00000010

typedef struct Message Message;
struct Message {
#ifdef MESSAGE_SENDER
    u32 header;
#endif
    s8 u_x;
    s8 u_y;
    s8 m_x;
    s8 m_y;
    u8 flags;
};

size_t message_print(Print & p, const Message & m) {
    size_t n = 0;
    n += p.print("{ u_x: ");
    n += p.print(m.u_x);
    n += p.print(" u_y: ");
    n += p.print(m.u_x);
    n += p.print(" m_x: ");
    n += p.print(m.m_x);
    n += p.print(" m_y: ");
    n += p.print(m.m_y);
    n += p.print(" laser: ");
    n += p.print(!!(m.flags & MESSAGE_LASER));
    n += p.print(" done: ");
    n += p.print(!!(m.flags & MESSAGE_DONE));
    return n;
}

enum state {
    header1 = 0x1,
    header2 = 0x2,
    header3 = 0x3,
    header4 = 0x4,
    data    = 0x5,
};

#endif /* PHASE_2_H */