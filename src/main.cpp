/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>


// Define motor control pins (adjust based on your wiring)
#define MOTOR1_IN1 12
#define MOTOR1_IN2 13
#define MOTOR2_IN1 14
#define MOTOR2_IN2 15
// Repeat for MOTOR3 and MOTOR4 as needed

void setup() {
    pinMode(MOTOR1_IN1, OUTPUT);
    pinMode(MOTOR1_IN2, OUTPUT);
    pinMode(MOTOR2_IN1, OUTPUT);
    pinMode(MOTOR2_IN2, OUTPUT);
}


void motorForward(int in1, int in2) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
}

void motorStop(int in1, int in2) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}

void loop() {
    // Test Motor 1 Forward
    motorForward(MOTOR1_IN1, MOTOR1_IN2);
    delay(2000);
    motorStop(MOTOR1_IN1, MOTOR1_IN2);
    delay(1000);

    // Test Motor 2 Forward
    motorForward(MOTOR2_IN1, MOTOR2_IN2);
    delay(2000);
    motorStop(MOTOR2_IN1, MOTOR2_IN2);
    delay(1000);
}