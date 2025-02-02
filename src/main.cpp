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

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif


// Data structure for joystick values
typedef struct struct_message {
    int x;
    int y;
    bool button;
} struct_message;

unsigned long last_receive = 0;


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
    last_receive = millis();
    struct_message joystick;
    memcpy(&joystick, incomingData, sizeof(joystick));
    Serial.println("OnDataRecv!");
    Serial.print("Received from: ");
    for (int i = 0; i < 6; i++) Serial.print(mac[i], HEX);
    Serial.println();
    Serial.printf("Received: X=%d, Y=%d, BTN=%d\n",
                  joystick.x, joystick.y, joystick.button);


    // Define deadzone for joystick center position
    const int DEADZONE = 200;

    // Stop if joystick is in center position (within deadzone)
    if (abs(joystick.x) < DEADZONE && abs(joystick.y) < DEADZONE) {
        stopAllMotors();
        return;
    }

    // Determine primary direction based on larger joystick axis value
    if (abs(joystick.y) > abs(joystick.x)) {
        // Forward/Backward movement takes priority
        if (joystick.y > DEADZONE) {
            // Forward
            setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, false);
            setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, false);
            setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, false);
            setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, false);
        } else if (joystick.y < -DEADZONE) {
            // Backward
            setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, true);
            setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, true);
            setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, true);
            setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, true);
        }
    } else {
        // Left/Right turning takes priority
        if (joystick.x > DEADZONE) {
            // Right turn
            setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, false);
            setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, true);
            setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, false);
            setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, true);
        } else if (joystick.x < -DEADZONE) {
            // Left turn
            setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, true);
            setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, false);
            setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, true);
            setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, false);
        }
    }

    // Optional: Emergency stop if joystick button is pressed
    if (joystick.button) {
        stopAllMotors();
    }
}


void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

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
    Serial.println("\n\nReceiver Starting...");
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    // Blink when no connection received in last 2 seconds
    if (millis() - last_receive > 2000) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(100);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    } else
        Serial.println("ESP-NOW initialized");
}