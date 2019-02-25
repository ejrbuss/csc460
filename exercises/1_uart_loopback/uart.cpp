/*
 * Exercise 1 - UART Loopback
 */

#define BUFFER_SIZE 30

void setup() {
    Serial.begin(14400);    // set baud rate for UART0
    Serial1.begin(14400);   // set baud rate for UART1
}

void loop() {

    char uart0_buffer[BUFFER_SIZE];
    char uart1_buffer[BUFFER_SIZE];

    if (Serial1.available > 0) {
        Serial1.readBytesUntil('\n', uart1_buffer, BUFFER_SIZE);
        Serial.write(uart1_buffer, BUFFER_SIZE);
    }

    if (Serial.available > 0) {
        Serial.readBytesUntil('\n', uart0_buffer, BUFFER_SIZE);
        Serial.write(uart0_buffer, BUFFER_SIZE);
    }

}