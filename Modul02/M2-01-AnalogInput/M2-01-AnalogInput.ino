/* Program : Analog Input
 * Membaca ADC yang terhubung ke 3 tombol
 * Ditampilkan ke OLED
 * Belajar:
 * - analogRead()
 * 
 * (c) Eko M. Budi, 2022
 */

// pustaka standar EScope
#include <TFScope22.h>

// pustaka I2C & OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BAUD 500000

// OLED --------------------------------
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

void setup(void)
{
  // siapkan Serial
  Serial.begin(BAUD);
  Serial.println("\nAnalog Read Button");
  
  // siapkan OLED
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED allocation failed");
    for (;;); // Don't proceed, loop forever
  }
  oled.setFont();
  oled.setTextColor(WHITE);

  // pesan ke layar serial
  Serial.println("\nTekan tombol berturutan, catat nilai AD-nya");
  Serial.println("BT0");
  Serial.println("BT1");
  Serial.println("BT2");
  Serial.println("BT0+BT1");
  Serial.println("BT0+BT2");
  Serial.println("BT0+BT3");
  Serial.println("BT0+BT2+BT3");  
}

void loop(void)
{
  // baca BT0 beberapa kali, agar teliti
  long sum = 0;
  for (int i=0; i<11; i++) {
    sum += analogRead(BT0);
  }  
  int ad = sum / 11;

  // konversi ke Volt
  // hanya perkiraan, tak terkalibrasi
  // Asumsi Vmax = 3,3 Volt
  int mv = (3300L * ad )/ AD_MAX;
  
  // tampilkan ke OLED
  oled.clearDisplay();

  oled.setTextSize(1);
  oled.setCursor(0, 0);  // Start at top-left corne
  oled.printf("mv= %d", mv);

  oled.setTextSize(2);
  oled.setCursor(0, 12);
  oled.printf("AD= %d", ad);
  
  oled.display();
  
  delay(200);
}
