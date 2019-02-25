/*
 *  Exercise 2 - Wireless UART Latency
 *  
 *  Measurements:
 *    0.121738625 s
 *    0.181070375 s
 *    0.210897875 s
 *    0.116067375 s
 *    0.140788250 s
 *    0.177663563 s
 *    0.233559625 s
 *    0.151308813 s
 *  Average: 0.1666368126 s
 */

#define BAUD_RATE 9600
#define MASTER    1

void setup() {
    Serial.begin(BAUD_RATE);    // Initialize UARTs
    Serial1.begin(BAUD_RATE); 
    pinMode(2, OUTPUT);         // Set pins as outputs  
    pinMode(3, OUTPUT);
    digitalWrite(2, LOW);       // Initialize pin values
    digitalWrite(3, LOW);
//    configure();              // Only ran once.

    if(MASTER) {
      // Send 32 bytes
      digitalWrite(2, HIGH);
      Serial1.write("01234567890123456789012345678901");
    }
}

void configure() {
    Serial.println("Starting configuration...\n");
    Serial1.write("AT");
    delay(5000);
    if(MASTER) {
      Serial1.write("AT+ROLE=M");  
    } else {
      Serial1.write("AT+ROLE=S");
    }
    delay(5000);
    Serial1.write("AT+BAUD4");
    delay(5000);
    Serial1.write("AT+PIN0003");
    delay(5000);
    Serial.println("Done configuration.\n");
}

void loop() {
    // Master - listen for the echo from Slave
    if(Serial1.available()) {
      digitalWrite(3, HIGH);
      Serial.write(Serial1.read());
      digitalWrite(3, LOW);
    }
//    // Slave - echo what is received back to Master
//    if(Serial1.available()) {
//        Serial1.write(Serial1.read());
//    }
}
