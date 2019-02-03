#pragma once

#ifndef PHASE_2_H
#define PHASE_2_H

#define MESSAGE_HEADER ((u64) 0x0807060504030201)

typedef struct Message Message;
struct Message {
#ifdef MESSAGE_SENDER
    u64 header;
#endif
    int u_x;
    int u_y;
    int m_x;
    int m_y;
    int laser;
    int done;
};

enum state {
    header1,
    header2,
    header3,
    header4,
    header5,
    header6,
    header7,
    header8,
    data
};

#endif /* PHASE_2_H */