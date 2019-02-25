/*
 * Exercise 4 - Joystick
 * 
 * Floating point arithmetic is slower than integer arithmetic.
 * However, more complex testing would need to occur in order
 * to draw concrete conclusions.
 */

// Joystick defines
#define PIN_X       2
#define PIN_Y       3
#define RESTING_X   525
#define RESTING_Y   513
#define DEAD_X_MAX  550
#define DEAD_X_MIN  450
#define DEAD_Y_MAX  560
#define DEAD_Y_MIN  480
#define MAX_DIST    512
#define ALPHA       0.3

// LED defines
#define MIN         0
#define MAX         255
#define LEDPIN      13
#define WAIT        (3000 / 2) / (MAX / 5)
#define MAX_INCR    130

void setup() {
  Serial.begin(9600);
}

double lowPassFilter(double x, double avg) {
  double average = (ALPHA * x) + ((1 - ALPHA) * avg);
  return average;
}

void loop() {
  static double average_x = 525;    // resting value
  static double average_y = 513;    // resting value
  static double x = 0;
  static double y = 0;
  static double dist = 0;
  static bool dir = false;          // direction of LED fade. true = brighter.
  static double incr = 5;
  static int i = MAX;
  
  x = analogRead(PIN_X);
  y = analogRead(PIN_Y);

  // Check dead band
  if ((x > 500) && (x < 600)) {
    x = RESTING_X;
  }
  if ((y > 500) && (y < 600)) {
    y = RESTING_Y;
  }

  // Apply low pass filter
  average_x = lowPassFilter(x, average_x);
  average_y = lowPassFilter(y, average_y);

  // Change LED breathing rate
  if (((average_x <= DEAD_X_MAX) && (average_x >= DEAD_X_MIN))
      && ((average_y <= DEAD_Y_MAX) && (average_y >= DEAD_Y_MIN))) {
    incr = 0;
    i = MAX;
  } else {
    dist = sqrt(pow(abs(average_x - RESTING_X), 2) + pow(abs(average_y - RESTING_Y), 2));
    if (dist > MAX_DIST) dist = MAX_DIST;
    incr = ((dist) / MAX_DIST) * MAX_INCR;

    if (dir) {
      i += incr;
    } else {
      i -= incr;
    }
  }

  // Clip the LED value to the (0,255) range
  if (i >= MAX) {
    i = MAX;
    dir = false;
  } else if (i <= MIN) {
    i = MIN;
    dir = true;
  }

  // Write to the LED
  analogWrite(LEDPIN, i);
  delay(WAIT);

  Serial.print("x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.print(y);
  Serial.print("  avg_x: ");
  Serial.print(average_x);
  Serial.print("  avg_y: ");
  Serial.print(average_y);
  Serial.print("  dir: ");
  Serial.print(dir);
  Serial.print("  i: ");
  Serial.println(i);
}
