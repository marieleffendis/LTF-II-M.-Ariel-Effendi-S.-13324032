#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <Stepper.h>

// Definisi Pin ESCOPE 2022
#define DO0 12
#define DO1 14
#define DO2 27
#define DO3 13
#define AO0 26

// Konfigurasi WiFi
const char* ssid = "RAIHAN_K1_02"; 
const char* password = "123456789";

// Inisialisasi Hardware
Servo myServo;
const int stepsPerRev = 2048; 

// Stepper menggunakan DO0, DO1, DO2, dan LED1 sebagai pin keempat
Stepper myStepper(stepsPerRev, DO0, DO2, DO1, AO0); 

AsyncWebServer server(80);

// Status Kontrol
enum ServoMode {S_STOP, S_MINUS, S_PLUS};
enum StepperMode {ST_STOP, ST_CW, ST_CCW};
ServoMode sState = S_STOP;
StepperMode stState = ST_STOP;

int servoPos = 90;
bool servoDir = true;
unsigned long lastServoMove = 0;

void setup() {
  Serial.begin(115200);
  
  // Setup Motor
  myServo.attach(DO3); // Servo sesuai permintaan pada DO3 (Pin 13)
  myServo.write(90); 
  myStepper.setSpeed(15); 

  WiFi.softAP(ssid, password);
  Serial.println("\n--- Sistem ESCOPE 2022 Aktif ---");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Jalur Perintah HTTP
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("cmd")) {
      String cmd = request->getParam("cmd")->value();
      
      if (cmd == "A") sState = S_MINUS;
      else if (cmd == "B") sState = S_PLUS;
      else if (cmd == "S") sState = S_STOP;
      else if (cmd == "X") stState = ST_CW;
      else if (cmd == "R") stState = ST_CCW;
      else if (cmd == "L") stState = ST_STOP;
      
      Serial.print("Menerima Perintah: ");
      Serial.println(cmd);
      request->send(200, "text/plain", "OK");
    }
  });

  server.begin();
}

void loop() {
  // Logika Gerak Servo bolak-balik
  if (sState != S_STOP) {
    if (millis() - lastServoMove > 15) {
      lastServoMove = millis();
      int minL = (sState == S_MINUS) ? 0 : 90;
      int maxL = (sState == S_MINUS) ? 90 : 180;
      
      if (servoDir) servoPos++; else servoPos--;
      if (servoPos >= maxL) servoDir = false;
      if (servoPos <= minL) servoDir = true;
      myServo.write(servoPos);
    }
  } else {
    myServo.write(90); 
  }

  // Logika Gerak Stepper
  if (stState == ST_CW) {
    myStepper.step(1);
  } else if (stState == ST_CCW) {
    myStepper.step(-1);
  }
}