#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // !!! THIS WILL ONLY WORK ON SOME DEVICES !!!
    /*
    DDRB  = 0xFF;
    PORTB = 0xFF;
    for (;;) {
        PORTB = ~0xFF;  
        _delay_ms(1000);
        PORTB = 0xFF;
        _delay_ms(1000);
    }
    */
    return 0;
}