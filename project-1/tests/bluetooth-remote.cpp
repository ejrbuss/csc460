#include "Base.h"
#include "Peripherals.h"

int main() {
    Serial1.begin(SERIAL_BAUD);
    for (;;) {
        if (Serial1.available()) {
            Serial1.write(Serial1.read());
        }
    }
    return 0;
}