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
    
    void print(Print & p, const Message_t * m) {
        p.print("{ u_x: ");
        p.print(m->u_x);
        p.print(" u_y: ");
        p.print(m->u_x);
        p.print(" m_x: ");
        p.print(m->m_x);
        p.print(" m_y: ");
        p.print(m->m_y);
        p.print(" laser: ");
        p.print(!!(m->flags & MESSAGE_LASER));
        p.print(" done: ");
        p.print(!!(m->flags & MESSAGE_DONE));
        p.print(" }\n");
    }

}
