#pragma once

#ifndef TEST_H
#define TEST_H

#include <RTOS.h>
#include "Peripherals.h"

#define assert(cond) (Test::_assert(cond, (#cond)))

namespace Test {

    typedef struct Schedule_t Schedule_t;
    struct Schedule_t {
        i64 time;
        const char * handle;
        volatile u8 instance;
    };

    static LiquidCrystal * lcd = nullptr;
    static Schedule_t * schedule;
    static u8 task_count = 0;

    void _assert(bool cond, const char * cond_token) {
        if (lcd == nullptr) {
            lcd = init_lcd();
        }
        if (cond) {
            lcd->print(".");
            RTOS::debug_print(".", 0);
        } else {
            lcd->print("!");
            lcd->setCursor(0, 1);
            lcd->print(cond_token);
            RTOS::debug_print("!\nAssert failed: %s", cond_token, 0);
            RTOS::halt();
        }
    }

    void set_schedule(Schedule_t * s) {
        schedule = s;
    }

    void schedule_trace(RTOS::Trace_t * trace) {
        switch (trace->tag) {
            case RTOS::Def_Task: {
                int i;
                for (i = 0; schedule[i].time != -1; i++) {
                    if (schedule[i].handle == trace->def.handle) {
                        schedule[i].instance = trace->def.task.instance;
                    }
                }
                break;
            }
            case RTOS::Mark_Start: {
                assert(schedule[task_count].instance == trace->mark.start.instance
                    && schedule[task_count].time == trace->mark.start.time
                );
                task_count++;
                if (schedule[task_count].time == -1) {
                    RTOS::debug_print("\nSchedule passed.\n");
                    RTOS::halt();
                }
                break;
            }
            default:
                break;
        }
    }

}

#endif /* TEST_H */