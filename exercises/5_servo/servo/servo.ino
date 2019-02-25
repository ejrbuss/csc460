/*
 * Exercise 5 - Servo Motor
 */

#include <Servo.h>

#define MIN 1000
#define MAX 2000
#define CENTER 1500
#define DELAY 5000 / (MAX - MIN)

Servo myservo;

void setup() {
  myservo.attach(9);
  pinMode(8, OUTPUT);   // this pin will indicate that the motor has completed one period
}

void loop() {
  static int pos = CENTER;
  delay(15);
  digitalWrite(8, HIGH);
  for (pos = CENTER; pos >= MIN; pos -= 2) {
    myservo.writeMicroseconds(pos);
    delay(DELAY);
  }
  for (pos = MIN; pos <= MAX; pos += 2) {
    myservo.writeMicroseconds(pos);
    delay(DELAY);
  }
  for (pos = MAX; pos >= CENTER; pos -= 2) {
    myservo.writeMicroseconds(pos);
    delay(DELAY);
  }
  digitalWrite(8, LOW);
}
