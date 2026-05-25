#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
String deviceName = "INDRA_K1_02";

#define BAUD 500000
#define SUDUT_MIN 0
#define SUDUT_MAX 180
#include <TFScope22.h>

int servo = DO0;
int sudut;
int t_pwm;

void rotateBasedOnAngle(int angle) {
  t_pwm = map(sudut, 0, 180, 500, 2500); // Konversi sudut ke t_pwm
  digitalWrite(servo, HIGH);
  delayMicroseconds(t_pwm);
  digitalWrite(servo, LOW);
  Serial.print("Mengubah sudut servo ke: ");
  Serial.println(angle);
}

void setup() {
  Serial.begin(BAUD);
  SerialBT.begin(deviceName); 
  Serial.println("");
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(servo, OUTPUT);
}

void loop() {
  if (SerialBT.available()) {
    String angleString = SerialBT.readString();
    int a = angleString.toFloat();
    int b = constrain(a, SUDUT_MIN, SUDUT_MAX);
    sudut = b;
  }
  rotateBasedOnAngle(sudut);
  delay(20);
}
