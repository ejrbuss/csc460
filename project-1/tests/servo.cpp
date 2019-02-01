#include "Base.h"
#include "Servo.h"

#define TOP    2000
#define BOTTOM 1000
#define CENTER 1500
#define DELAY  5000./ 1000

Servo myservo;  // create servo object to control a servo


int main() {
    init_arduino();
    // attaches the servo on pin 9 to the servo object
    myservo.attach(13);  
    for (;;) {
        int pos = CENTER;
        // Rotate the servo
        for (pos = CENTER; pos >= BOTTOM; pos -= 2) {
            myservo.writeMicroseconds(pos);
            delay(DELAY);   
        }
        for (pos = BOTTOM; pos <= TOP; pos += 2) {
            myservo.writeMicroseconds(pos);
            delay(DELAY);
        }
        for (pos = TOP; pos >= CENTER; pos -= 2) {
            myservo.writeMicroseconds(pos);
            delay(DELAY);
        }
    }
}