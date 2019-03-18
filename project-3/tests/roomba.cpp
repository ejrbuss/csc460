#include <RTOS.h>
#include "Roomba.h"

#define BAUD 9600
#define ROOMBA_UART 2
#define BAUD_RATE_CHANGE_PIN 30

using namespace RTOS;

int main() {
    RTOS::init();
    // RTOS::dispatch();
    
    Roomba r(ROOMBA_UART, BAUD_RATE_CHANGE_PIN);
    
    Serial1.begin(BAUD);
    r.init();
    delay(1000);
    
    bool initialized = true;
    
    //Read commands from Serial.
    while (1) {
        if(Serial1.available()) {

            if(!initialized) {
                r.init();
                initialized = true;
            }

            //Make sure the Roomba is ready to go.
            int command = Serial1.read();
            switch(command)
            {
            case 'f': 
                r.drive(150, 32768);
                break;
            case 'b':
                r.drive(-150, 32768);
                break;
            case 'r':
                r.drive(50, -1);
                break;
            case 'l':
                r.drive(50, 1);
                break;
            case 's':
                r.drive(0,0);
                break;
            case 'd':
                r.dock();
                break;
            case 'p':
                r.power_off();
                initialized = false;
                break;
            default:
                break;
            }
        }
        
        delay(100);
    }
    
    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * trace) {
        return;
    }

    bool error(Trace_t * trace) {
        return true;
    }

}}