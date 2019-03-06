#pragma once

#ifndef TEST_H
#define TEST_H

#include <RTOS.h>
#include "Peripherals.h"

#define assert(cond) (Test::_assert(cond, (#cond)))

namespace Test {

    typedef struct Schedule_t Schedule_t;
    struct Schedule_t {
        u64 time;
        const char * handle;
        u8 instance;
    };

    static LiquidCrystal * lcd;
    static Schedule_t * schedule_list;
    static u8 schedule_num = 0;

    void init() {
        lcd = init_lcd();
    }

    void _assert(bool cond, const char * cond_token) {
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

    void schedule(Schedule_t * list) {
        Test::init();
        schedule_list = list;
    }

    void schedule_trace(RTOS::Trace_t * trace) {
        switch (trace->tag) {
            case RTOS::Def_Task: {
                int i;
                for (i = 0; schedule_list[i].time != 0; i++) {
                    if (schedule_list[i].handle == trace->def.handle) {
                        schedule_list[i].instance = trace->def.task.instance;
                    }
                }
                break;
            }
            case RTOS::Mark_Start: {
                assert(schedule_list[schedule_num].instance == trace->mark.start.instance
                    && schedule_list[schedule_num].time == trace->mark.start.time
                );
                schedule_num++;
                if (schedule_list[schedule_num].time == 0) {
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