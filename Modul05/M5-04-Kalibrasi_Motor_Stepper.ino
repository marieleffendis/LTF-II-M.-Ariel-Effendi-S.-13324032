/*M5-03-Kalibrasi_Motor_Stepper
 * Program menggerakkan jarum motor stepper menggunakan tombol BT0/BT1/BT2 untuk kalibrasi (menentukan koreksi)
 * - menggunakan library Servo.h
 * - BT0/BT1/BT2 menggerakkan jarum sebesar 15 derajat berlawanan arah jam
 * - catat posisi awal jarum seperti yang dapat dilihat di busur derajat
 * - tekan tombol reset terlebih dahulu di ESP32 (tombol EN) untuk mengembalikan count ke 0
 * - tekan tombol BT0/BT1/BT2 berkali-kali hingga jarum berputar 360 derajat (periksa pada motor maupun pada Serial Monitor)
 * - jika gerak jarum dan besar sudut tidak sesuai, cari/hitung faktor koreksinya dan perbaiki pada fungsi step_Degree()
 * 
 * Copyright: Revisi oleh Jhon Christabel Fausta Silalahi - 13323054
 */

#include <Stepper.h>
#include <TFScope22.h> 

#define BAUD 500000

const float stepsPerRevolution = 2048;  // jumlah langkah per satu putaran penuh
const float revolution = 360; // 1 revolusi penuh 360 derajat

// Pin drive motor ULN2003 
#define IN1 DO0
#define IN2 DO1
#define IN3 DO2
#define IN4 DO3

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4); 

float step_angle = revolution / stepsPerRevolution; 
float steps;
int count = 0; // Variabel penghitung global
                                                                                                                    
// Treshold analog button (Ubah sesuai dengan threshold masing-masing board)
int ab_tresholds[] = { 2100, 2400, 2600 };
AnalogButton _btn(ab_tresholds);

byte bt_bin;
byte last_bt_bin = 0; // Untuk mendeteksi perubahan status tombol

// Konversi dari sudut menjadi step atau langkah putaran motor
void step_Degree(float angle){
  float koreksi = 1; // Faktor koreksi empiris (ini yang diubah-ubah)
  steps = angle / step_angle * koreksi;
  myStepper.step(steps);
}

void setup() {
  Serial.begin(BAUD);
  myStepper.setSpeed(10);
  Serial.println("Sistem Siap. Tekan tombol mana saja untuk kalibrasi 15 derajat.");
}

void loop() {
  int sudut = 15;
  
  // Membaca input nilai analog dari tombol
  int bt_analog = _btn.aRead();
  bt_bin = _btn.a2d(bt_analog); // Konversi nilai analog ke biner digital (1, 2, atau 4)

  // LOGIKA SEKALI TEKAN: 
  // Mengecek apakah tombol SEDANG ditekan (bt_bin != 0) DAN SEBELUMNYA tidak ditekan (last_bt_bin == 0)
  if (bt_bin != 0 && last_bt_bin == 0) {
      
      step_Degree(sudut); 
      count += 1;
      
      Serial.println();
      Serial.printf("Tombol telah ditekan sebanyak: %u kali\n", count);
      Serial.printf("Jumlah steps: %.2f step\n", count * steps);
      Serial.printf("Jarum sudah bergerak sejauh: %u derajat\n", count * sudut);
      
      /*
      // SOLUSI MOTOR PANAS: Memutuskan arus ke semua kumparan setelah bergerak
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW); 
      */
  }
  
  // Memperbarui status tombol sebelumnya dengan status tombol saat ini
  last_bt_bin = bt_bin;
  
  // Jeda sangat singkat (50ms) murni untuk stabilitas sensor analog ADC
  delay(50); 
}
