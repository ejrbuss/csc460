#pragma once

#ifndef PHASE_2_H
#define PHASE_2_H

typedef struct Message Message;
struct Message {
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