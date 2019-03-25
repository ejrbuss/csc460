#include <RTOS.h>
#include "Message.h"

enum state {
    header1 = 0x1,
    header2 = 0x2,
    header3 = 0x3,
    header4 = 0x4,
    data    = 0x5,
};

namespace Message {

    void send(Message_t * current_message) {
        // Write each byte of current message one at a time
        u8 * buffer = (u8 *) &current_message;
        u16 i;
        for (i = 0; i < sizeof(Message_t); i++) {
            Serial1.write(buffer[i]);
        }
    }

    Message_t * receive(Message_t * current_message) {
        static int i = 0;
        static int state = header1;
        Message_t buffer_message;

        if (current_message == NULL) {
            u8 * buffer = (u8 *) &buffer_message;
            while (Serial1.available()) {
                switch(state) {
                    case header1:
                    case header2:
                    case header3:
                    case header4:
                        if (Serial1.read() == state) {
                            state++;
                        } else {
                            state = header1;
                        }
                        break;
                    case data:
                        // Read as much as is available
                        while (Serial1.available()) {
                            buffer[i++] = Serial1.read();
                            if (i == sizeof(Message_t)) {
                                current_message = &buffer_message;
                                state = header1;
                                i = 0;
                                return current_message;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        return current_message;
    }

}
