/*
 * Exercise 3 - PWM and LED 
 * 
 * Accompanying Screenshots:
 *  logic1 - relatively small duty cycle, dimmer LED
 *  logic2 - relatively big duty cycle, brighter LED
 */

#define MIN     0
#define MAX     255
#define LEDPIN  13
#define INCR    5
#define WAIT    (3000 / 2) / (MAX / INCR)   // calculate delay for breathing period

void setup() {
  // nothing
}

void loop() {
  for(int i = MIN; i <= MAX; i += INCR) {
    analogWrite(LEDPIN, i);
    delay(WAIT);
  }
  for(int i = MAX; i >= MIN; i -= INCR) {
    analogWrite(LEDPIN, i);
    delay(WAIT);
  }
}
