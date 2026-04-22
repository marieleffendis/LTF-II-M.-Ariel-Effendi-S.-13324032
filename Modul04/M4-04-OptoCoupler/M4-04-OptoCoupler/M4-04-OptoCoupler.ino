/* Program : Opto Coupler
 * Test Optocoupler Sensor
 * Dengan memakai interrupt untuk counter
 * 
 * Ref: https://www.theengineeringprojects.com/2021/12/esp32-interrupts.html
 * 
 * (c) Eko M. Budi, 2022
 */

#include <TFScope22.h>

#define DI_OPTO DI0

volatile unsigned opto_counter = 0;
portMUX_TYPE opto_mux = portMUX_INITIALIZER_UNLOCKED;
 
void optoISR() {
  portENTER_CRITICAL_ISR(&opto_mux);
  opto_counter++;
  portEXIT_CRITICAL_ISR(&opto_mux);
}

unsigned optoCount() {
  unsigned count;
  portENTER_CRITICAL_ISR(&opto_mux);
  count = opto_counter;
  portEXIT_CRITICAL_ISR(&opto_mux);
  return count;
}

unsigned optoReset() {
  unsigned count;
  portENTER_CRITICAL_ISR(&opto_mux);
  count = opto_counter;
  opto_counter=0;
  portEXIT_CRITICAL_ISR(&opto_mux);
  return count;
}

void optoStart() {
  opto_counter=0;
  pinMode(DI_OPTO, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DI_OPTO), optoISR, FALLING); 
}

void optoStop() {
  detachInterrupt(digitalPinToInterrupt(DI_OPTO)); 
}

void plotSignal() {
  bool d = digitalRead(DI_OPTO);
  Serial.println(d);    
}

void plotHeader() {
  Serial.print("Pulse Counter=");  
  Serial.println(optoCount());
}

void setup() {
  Serial.begin(500000);
  delay(1000);
  Serial.println();
  Serial.println("OptoCoupler Test");
  optoStart();
}

void loop() {
  plotHeader();
  plotSignal();
  delay(10);  
}
