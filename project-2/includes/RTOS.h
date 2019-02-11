#pragma once

#ifndef RTOS_H
#define RTOS_H

#include "Arduino.h"
#include "Types.h"


typedef struct TaskMetrics TaskMetrics;
struct TaskMetrics {
    u16 average_compute;
    u16 remaining;
};

typedef struct Task Task;
struct Task {
    char * name;
    void * state;
    u16 instance;
    u16 period;
    u16 offset;
    i16 (*fn)(Task * self);
    TaskMetrics metrics;
};

Task * get_Task(size_t state_size);

void dispatch(Task * task);

void handoff();

#endif /* RTOS_H */