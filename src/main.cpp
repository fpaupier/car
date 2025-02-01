#include <esp_now.h>
#include <WiFi.h>
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

// Data structure for joystick values
typedef struct struct_message {
    int x;
    int y;
    bool button;
} struct_message;


// Motor control functions
void setMotor(int in1, int in2, bool forward) {
    digitalWrite(in1, forward ? LOW : HIGH);
    digitalWrite(in2, forward ? HIGH : LOW);
}

void stopAllMotors() {
    digitalWrite(REAR_LEFT_IN1, LOW);
    digitalWrite(REAR_LEFT_IN2, LOW);
    digitalWrite(REAR_RIGHT_IN3, LOW);
    digitalWrite(REAR_RIGHT_IN4, LOW);
    digitalWrite(FRONT_LEFT_IN1, LOW);
    digitalWrite(FRONT_LEFT_IN2, LOW);
    digitalWrite(FRONT_RIGHT_IN3, LOW);
    digitalWrite(FRONT_RIGHT_IN4, LOW);
}

// ESP-NOW callback
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    Serial.println("Data received");
    struct_message joystick;
    memcpy(&joystick, incomingData, sizeof(joystick));

    // Define deadzone to prevent motor jitter
    const int DEADZONE = 100;

    // Normalize joystick values to -100 to +100 range
    int xValue = map(joystick.x, -2048, 2048, -100, 100);
    int yValue = map(joystick.y, -2048, 2048, -100, 100);

    // Apply deadzone
    if (abs(xValue) < DEADZONE) xValue = 0;
    if (abs(yValue) < DEADZONE) yValue = 0;

    // Stop if both values are in deadzone
    if (xValue == 0 && yValue == 0) {
        stopAllMotors();
        return;
    }

    // Calculate left and right motor directions
    bool leftForward, rightForward;

    // Forward/Backward motion
    if (abs(yValue) > abs(xValue)) {
        if (yValue > 0) {  // Forward
            leftForward = true;
            rightForward = true;
        } else {  // Backward
            leftForward = false;
            rightForward = false;
        }

        // Add turning influence while moving
        if (xValue > 0) {  // Right turn
            rightForward = !rightForward;  // Reverse right motors
        } else if (xValue < 0) {  // Left turn
            leftForward = !leftForward;   // Reverse left motors
        }
    }
        // Left/Right rotation
    else {
        if (xValue > 0) {  // Turn right
            leftForward = true;
            rightForward = false;
        } else {  // Turn left
            leftForward = false;
            rightForward = true;
        }
    }

    // Apply motor directions
    // Left side motors
    setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, leftForward);
    setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, leftForward);

    // Right side motors
    setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, rightForward);
    setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, rightForward);
}


void setup() {
    // Initialize motor pins
    pinMode(REAR_LEFT_IN1, OUTPUT);
    pinMode(REAR_LEFT_IN2, OUTPUT);
    pinMode(REAR_RIGHT_IN3, OUTPUT);
    pinMode(REAR_RIGHT_IN4, OUTPUT);
    pinMode(FRONT_LEFT_IN1, OUTPUT);
    pinMode(FRONT_LEFT_IN2, OUTPUT);
    pinMode(FRONT_RIGHT_IN3, OUTPUT);
    pinMode(FRONT_RIGHT_IN4, OUTPUT);
    stopAllMotors();

    // Init ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    Serial.println("ESP-NOW initialized");

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // Empty - everything handled in callbacks
}