#include "Base.h"
#include <RTOS.h>

int led = 13;
 
int main() {
    init_arduino();
    // initialize the digital pin as an output.
    pinMode(led, OUTPUT);    
    for (;;) {
        digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);               // wait for a second
        digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
        delay(1000);               // wait for a second
    }
    return 0;
}