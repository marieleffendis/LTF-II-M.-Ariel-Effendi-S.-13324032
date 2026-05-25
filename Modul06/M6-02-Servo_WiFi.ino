#include "WiFi.h"
#include "ESPAsyncWebServer.h"

const char* ssid = "INDRA_K1_02";
const char* password = "beruangmakanikan";


#define SUDUT_MIN -90
#define SUDUT_MAX 90
#include <TFScope22.h>

AsyncWebServer server(80);

int servo = DO0;
int sudut = 0;
int t_pwm;

void rotateBasedOnAngle(int angle) {
  t_pwm = map(sudut, -90, 90, 500, 2500); // Konversi sudut ke t_pwm
  digitalWrite(servo, HIGH);
  delayMicroseconds(t_pwm);
  digitalWrite(servo, LOW);
  Serial.print("Mengubah sudut servo ke: ");
  Serial.println(angle);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  pinMode(servo, OUTPUT);

  Serial.println("Setting AP (Access Point)...");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("angle")) {
      String angleString = request->getParam("angle")->value();
      request->send(200, "text/plain", "Input angle received: " + angleString);

      int a = angleString.toFloat();
      int b = constrain(a, SUDUT_MIN, SUDUT_MAX);
      sudut = b;
      
    } else {
      request->send(400, "text/plain", "Bad Request: No message content provided");
    }
  });
 
  server.begin();
}

void loop() {
 rotateBasedOnAngle(sudut);
 delay(20);
}
