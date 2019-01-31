#pragma once

#ifndef COMPUTE_H
#define COMPUTE_H

#include "Base.h"

// [-128.996, 127.996]
#define Q78_SCALE_FACTOR 0x0100

// Q7.8
typedef s16 Q78_t;

#define Q78(n) ((Q78_t) (n * Q78_SCALE_FACTOR))

int Q78_to_int(Q78_t q);
float Q78_to_float(Q78_t q);

Q78_t Q78_mul(Q78_t a, Q78_t b);
Q78_t Q78_div(Q78_t a, Q78_t b);

Q78_t Q78_lpf(Q78_t sample, Q78_t average, Q78_t factor);

int clamp(int value, int min_value, int max_value);


#endif /* COMPUTE_H */