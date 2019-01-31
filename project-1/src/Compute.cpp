#include "Compute.h"

int Q78_to_int(Q78_t q) {
    return (int) (q / Q78_SCALE_FACTOR);
}

float Q78_to_float(Q78_t q) {
    return ((float) q) / Q78_SCALE_FACTOR;
}

Q78_t Q78_mul(Q78_t a, Q78_t b) {
    s32 tmp_a = a;
    s32 tmp_b = b;
    s32 tmp_c = (tmp_a * tmp_b) / Q78_SCALE_FACTOR;
    return (Q78_t) tmp_c;
}

Q78_t Q78_div(Q78_t a, Q78_t b) {
    s32 tmp_a = a;
    s32 tmp_b = b;
    s32 tmp_c = (tmp_a * Q78_SCALE_FACTOR) / tmp_b;
    return (Q78_t) tmp_c;
}

Q78_t Q78_lpf(Q78_t sample, Q78_t average, Q78_t factor) {
    static Q78_t one = Q78(1);
    return Q78_mul(sample, factor) + Q78_mul(average, (one - factor));
}

int clamp(int value, int min_value, int max_value) {
    int tmp = value < min_value ? min_value : value;
    return tmp > max_value ? max_value : tmp;
}
