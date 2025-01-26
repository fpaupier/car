#include <Arduino.h>

// Rear Motors (L298N 1)
#define REAR_LEFT_IN1  12
#define REAR_LEFT_IN2  13
#define REAR_RIGHT_IN3 14
#define REAR_RIGHT_IN4 15

// Front Motors (L298N 2)
#define FRONT_LEFT_IN1 16
#define FRONT_LEFT_IN2 17
#define FRONT_RIGHT_IN3 18
#define FRONT_RIGHT_IN4 19

void setup() {
  // Initialize all motor control pins
  pinMode(REAR_LEFT_IN1, OUTPUT);
  pinMode(REAR_LEFT_IN2, OUTPUT);
  pinMode(REAR_RIGHT_IN3, OUTPUT);
  pinMode(REAR_RIGHT_IN4, OUTPUT);
  pinMode(FRONT_LEFT_IN1, OUTPUT);
  pinMode(FRONT_LEFT_IN2, OUTPUT);
  pinMode(FRONT_RIGHT_IN3, OUTPUT);
  pinMode(FRONT_RIGHT_IN4, OUTPUT);
}

// Motor control functions
void setMotor(int in1, int in2, bool forward) {
  digitalWrite(in1, forward ? HIGH : LOW);
  digitalWrite(in2, forward ? LOW : HIGH);
}

void stopMotor(int in1, int in2) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

// Test all motors
void testAllMotors() {
  // All motors forward (2 seconds)
  setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, true);
  setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, true);
  setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, true);
  setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, true);
  delay(2000);

  // Stop all motors (1 second)
  stopMotor(REAR_LEFT_IN1, REAR_LEFT_IN2);
  stopMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4);
  stopMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2);
  stopMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4);
  delay(1000);
}

void loop() {
  testAllMotors();
}