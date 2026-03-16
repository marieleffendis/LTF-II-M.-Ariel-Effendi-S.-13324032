/* Program : DA_INA_AD
 * DA mengeluarkan sinyal ramp
 * INA dan AD mengukur
 * Print ke Serial Monitor, format siap sbg array
 * (c) Eko M. Budi, 2022
 */

// pustaka
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <TFScope22.h>

#define BAUD 500000

// Pin Standar kit ESCOPE 2020 ESP32
// sudah terinclude di library TFScope22

#define DA_MAX 255
#define AD_MAX 4095

// definisi pin, taruh di array
#define N_AD 2
int p_ad[N_AD] = {AI0, AI1};
int p_da = AO0;

// pesan obyek INA219
Adafruit_INA219 ina219;

// data
int da_out; 
int ad_in;
int ina_mv;

// Fungsi untuk print data ke serial monitor
void printData() {
  char buff[40];
  // format ke spreadsheet
  sprintf(buff,"%4d, %4d, %4d", ad_in, ina_mv, da_out);
  // format ke array
  // sprintf(buff,"{%4d, %4d, %4d},",  ad_in, ina_mv, da_out);
  Serial.print(buff);
}

// UTAMA
void setup() {
  // Siapkan serial, tunggu sampai terkoneksi
  Serial.begin(BAUD);
  while (!Serial) {}
  Serial.println("\nAnalog I/O");  

  // siapkan INA219
  if (!ina219.begin()) {
    while (1) { delay(100); }
  }  
}

void loop() {
  // Header Plot
  Serial.println("AD  Vi+(mV) DA");

  // mengeluarkan output ramp untuk kalibrasi
  for (da_out=0; da_out<=DA_MAX; da_out++) {
    // keluarkan sinyal ke DA
    dacWrite(AO0, da_out);
    delay(10);    // tunggu tunak

    // baca AD
    ad_in = analogRead(AI0);

    // baca INA 
    float shunt_mv = ina219.getShuntVoltage_mV();
    float bus_v = ina219.getBusVoltage_V();    

    // konversi bacaan ina ke tegangan (mV)
    ina_mv = shunt_mv + bus_v * 1000; // INI MASIH SALAH !!!

    Serial.print("{");
    printData();    
    Serial.println("},");
  }
  
  // tunggu sampai dikirimi sesuatu dari Serial monitor
  Serial.println("Kirim sembarang huruf untuk ukur lagi");
  while(!Serial.available());
  while(Serial.available()) Serial.read();
}
