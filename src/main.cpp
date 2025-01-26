#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "secrets.h"

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


// WiFi credentials
const char* ssid = SSID_NAME;
const char* password = SSID_PASSWORD;

WebServer server(80);

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

void handleRoot() {
    String html = R"HTML(
  <!DOCTYPE html>
  <html>
  <head>
    <title>4WD Control</title>
    <style>
      .container {text-align: center; margin-top: 50px;}
      .button {
        padding: 20px;
        font-size: 20px;
        margin: 5px;
        border-radius: 12px;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>4WD Control</h1>
      <button class="button" onclick="move('forward')">&uarr; Forward</button><br>
      <button class="button" onclick="move('left')">&larr; Left</button>
      <button class="button" onclick="move('stop')">STOP</button>
      <button class="button" onclick="move('right')">&rarr; Right</button><br>
      <button class="button" onclick="move('backward')">&darr; Backward</button>
    </div>
    <script>
      function move(direction) {
        fetch('/' + direction).then(r => console.log(r.status))
      }
    </script>
  </body>
  </html>
  )HTML"; // Matches the custom delimiter "HTML"


    server.send(200, "text/html", html);
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

    // Connect to WiFi
    Serial.begin(115200); // Initialize serial communication
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP()); // Print the IP address

    // Set up web server
    server.on("/", handleRoot);
    server.on("/forward", []() {
        // All motors forward
        setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, false);
        setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, false);
        setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, false);
        setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, false);
        server.send(200);
    });
    server.on("/backward", []() {
        // All motors backward
        setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, true);
        setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, true);
        setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, true);
        setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, true);
        server.send(200);
    });
    server.on("/left", []() {
        // Right side forward, left side backward
        setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, true);
        setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, false);
        setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, true);
        setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, false);

        server.send(200);
    });
    server.on("/right", []() {
        // Left side forward, rigt side backward
        setMotor(REAR_LEFT_IN1, REAR_LEFT_IN2, false);
        setMotor(REAR_RIGHT_IN3, REAR_RIGHT_IN4, true);
        setMotor(FRONT_LEFT_IN1, FRONT_LEFT_IN2, false);
        setMotor(FRONT_RIGHT_IN3, FRONT_RIGHT_IN4, true);
        server.send(200);
    });
    server.on("/stop", []() {
        stopAllMotors();
        server.send(200);
    });

    server.begin();
}

void loop() {
    server.handleClient();
}