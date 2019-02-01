#pragma once

#ifndef PHASE_2_H
#define PHASE_2_H

typedef struct Message Message;
struct Message {
    int u_x;
    int u_y;
    int m_x;
    int m_y;
    char laser;
    char done;
};

#endif /* PHASE_2_H */