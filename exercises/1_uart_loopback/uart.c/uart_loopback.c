/*
 *  Exercise 1 - UART Loopback
 *
 *  Disconnect symbol: Ctrl+D or *
 *  The number of bytes sent will be one higher than the number of bytes
 *  received because I count sending the disconnect symbol, which isn't
 *  returned through the loopback.
 *
 *  Findings on baud rates:
 *  Using different baud rates affects how fast data is transmitted.
 *  A lower baud rate means a fewer number of symbols can be expressed in
 *  one second. Therefore, with a low baud rate, there was a noticable delay
 *  between when each character was returned through the UART and displayed.
 *  19200 baud did not exhibit this delay.
 */

#define BAUD_RATE 19200

uint16_t totalBytesSent = 0;
uint16_t totalBytesReceived = 0;
uint8_t  width = 0;

void setup() {
    Serial.begin(BAUD_RATE);    // set baud rate for UART0
    Serial1.begin(BAUD_RATE);   // set baud rate for UART1
}

void writebyte(byte b) {
    if (b == '*' || b == 0x04) {                // disconnect symbol found
        Serial.print("\nTotal bytes sent: ");
        Serial.println(totalBytesSent);
        Serial.print("Total bytes received: ");
        Serial.println(totalBytesReceived);
        Serial1.end();
        Serial.end();
    } else if (b == '\n' || b == '\r') {        // newline sent, reset width
        width = 0;
    } else if (width == 30) {                   // reached justified width
        Serial.write('\n');
        width = 1;
    } else {
        width ++;
    }

    Serial.write(b);
    totalBytesReceived ++;
}

void loop() {
    char uart0_rx_byte;
    char uart1_rx_byte;

    if (Serial1.available() > 0) {
        uart1_rx_byte = Serial1.read();
        totalBytesSent ++;
        writebyte(uart1_rx_byte);
    }

    if (Serial.available() > 0) {
        uart0_rx_byte = Serial.read();
        Serial1.write(uart0_rx_byte);
    }
}
