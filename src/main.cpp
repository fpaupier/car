#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include <driver/ledc.h>


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

#define TIMEOUT_MS      2000

// PWM Configuration
#define PWM_FREQ        5000
#define PWM_RESOLUTION  8

// Add enable pin definitions
#define ENA_FRONT 27
#define ENB_FRONT 26
#define ENA_REAR  25
#define ENB_REAR  32

#define PWM_CHANNEL_FL  0
#define PWM_CHANNEL_FR  1
#define PWM_CHANNEL_RL  2
#define PWM_CHANNEL_RR  3

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif


// Data structure for joystick values
typedef struct struct_message {
    int x;
    int y;
    bool button;
} struct_message;

unsigned long lastReceiveTime = 0;


// Motor control functions

void setMotor(int channel, int pinA, int pinB, int speed) {
    bool forward = speed >= 0;
    int absSpeed = abs(speed);

    digitalWrite(pinA, forward ? HIGH : LOW);
    digitalWrite(pinB, forward ? LOW : HIGH);
    ledcWrite(channel, absSpeed);
}

void controlMotors(int left, int right) {
    Serial.printf("Control motors left: %4d | Right: %4d\n", left, right);
    setMotor(PWM_CHANNEL_RL, REAR_LEFT_IN1, REAR_LEFT_IN2, left);
    setMotor(PWM_CHANNEL_FL, FRONT_LEFT_IN1, FRONT_LEFT_IN2, left);
    setMotor(PWM_CHANNEL_RR, REAR_RIGHT_IN3, REAR_RIGHT_IN4, right);
    setMotor(PWM_CHANNEL_FR, FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, right);
}


void stopAllMotors() {
    ledcWrite(PWM_CHANNEL_RL, 0);
    ledcWrite(PWM_CHANNEL_RR, 0);
    ledcWrite(PWM_CHANNEL_FL, 0);
    ledcWrite(PWM_CHANNEL_FR, 0);
}

void setupMotors() {
    pinMode(ENA_FRONT, OUTPUT);
    pinMode(ENB_FRONT, OUTPUT);
    pinMode(ENA_REAR, OUTPUT);
    pinMode(ENB_REAR, OUTPUT);
    pinMode(REAR_LEFT_IN1, OUTPUT);
    pinMode(REAR_LEFT_IN2, OUTPUT);
    pinMode(REAR_RIGHT_IN3, OUTPUT);
    pinMode(REAR_RIGHT_IN4, OUTPUT);
    pinMode(FRONT_LEFT_IN1, OUTPUT);
    pinMode(FRONT_LEFT_IN2, OUTPUT);
    pinMode(FRONT_RIGHT_IN3, OUTPUT);
    pinMode(FRONT_RIGHT_IN4, OUTPUT);
}

void setupPWM() {
    // Setup PWM channels
    ledcSetup(PWM_CHANNEL_FL, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_FR, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_RL, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_RR, PWM_FREQ, PWM_RESOLUTION);

    // Attach PWM to enable pins
    ledcAttachPin(ENA_FRONT, PWM_CHANNEL_FL);
    ledcAttachPin(ENB_FRONT, PWM_CHANNEL_FR);
    ledcAttachPin(ENA_REAR, PWM_CHANNEL_RL);
    ledcAttachPin(ENB_REAR, PWM_CHANNEL_RR);
}

// ESP-NOW callback
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    struct_message joystickData;
    memcpy(&joystickData, incomingData, sizeof(joystickData));
    Serial.printf("X: %4d | Y: %4d | BTN: %d\n", joystickData.x, joystickData.y, joystickData.button);
    lastReceiveTime = millis();

    // Calculate motor speeds
    int leftSpeed = constrain(joystickData.y + joystickData.x, -255, 255);
    int rightSpeed = constrain(joystickData.y - joystickData.x, -255, 255);

    controlMotors(leftSpeed, rightSpeed);
}


void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    setupMotors();
    setupPWM();

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
    unsigned long currentTime = millis();
    if (currentTime - lastReceiveTime > TIMEOUT_MS) {
        stopAllMotors();
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(100);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
}