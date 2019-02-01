#include "Base.h"
#include "Peripherals.h"

#define BAUD      9600
#define PHOTO_PIN 13

int main() {
    init_arduino();
    pinMode(PHOTO_PIN, INPUT);
    Serial.begin(BAUD);

    for(;;) {
        Serial.println(analogRead(PHOTO_PIN));
    }
}