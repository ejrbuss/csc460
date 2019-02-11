#define LOGIC

#include "Base.h"
#include "Peripherals.h"

#define BUF_LEN     80
#define PIN_NOINT   42
#define TIMER_COUNT 250

u64 millis_passed = 0;
u8 changed        = YES;

void print_time(LiquidCrystal * lcd) {
    static int run = 0;
    static char buffer[BUF_LEN];
    int minutes    = (millis_passed / (1000 * 60)) % 100;
    int seconds    = (millis_passed / (1000))      % 100;
    int hundredths = (millis_passed / (10))        % 100;
    snprintf(buffer, BUF_LEN, "%02d:%02d:%02d", minutes, seconds, hundredths);
    lcd->clear();
    lcd->print(buffer);
    lcd->print("  ");
    lcd->print(run++);
}

ISR (TIMER1_COMPA_vect) {
    static u8 milli_count = 0;
    
    // disable interrupts
    noInterrupts();
    LOGIC_HIGH(PIN_NOINT);

    // Change time
    milli_count++;
    millis_passed++;
    if (milli_count == 10) {
        milli_count = 0;
        changed = YES;
    }

    // enable interrupts
    interrupts();
    LOGIC_LOW(PIN_NOINT);
}

int main() {
    init_arduino();
    LOGIC_INIT(PIN_NOINT);
    LiquidCrystal lcd = init_lcd();

    // Begin timer ISR configuration
    noInterrupts(); {
        // We are using timer 1
        // Configuring timer counter control register A and B
        TCCR1A = 0x00;                 // Clear control register A
        TCCR1B = 0x00;                 // Clear control register B
        TCNT1  = 0x00;                 // Clear the counter
        OCR1A  = TIMER_COUNT;          // The value we are waiting for
        TCCR1B |= BV(WGM12);           // Use CTC mode
        TCCR1B |= BV(CS11) | BV(CS10); // Scale by 64
        TIMSK1 |= BV(OCIE1A);          // Enable timer compare interrupt
    } interrupts(); 

    for (;;) {
        if (changed) {
            print_time(&lcd);
            changed = NO;
        }
    }
    return 0;
}

