#include "Base.h"
#include "Peripherals.h"


int main() {
    init_arduino();
    pinMode(PHOTO_PIN, INPUT);
    Serial.begin(SERIAL_BAUD);

    for(;;) {
        Serial.println(analogRead(PHOTO_PIN));
    }
}