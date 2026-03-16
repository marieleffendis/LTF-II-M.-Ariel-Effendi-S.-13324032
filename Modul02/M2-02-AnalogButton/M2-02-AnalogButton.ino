/* Program : Analog Button
 * Membaca analog Button dan menampilkan ke LED
 * Belajar:
 * - class AnalogButton dan MultiLED
 * - memilah tegangan jadi bacaan 3 button
 * - menampilkan ke LED
 * 
 * (c) Eko M. Budi, 2022
 */

// pustaka I2C & OLED
#include <TFScope22.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define BAUD 500000

// treshold analog button (hasil M2-01)
int ab_tresholds[] = {
  2036,2390,2587,2850,2961,3075,3275  
};

// kelas analog button
AnalogButton _btn(ab_tresholds);
MultiLED _led;

// OLED --------------------------------
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

void setup(void)
{
  // siapkan Serial
  Serial.begin(BAUD);
  Serial.println("\nAnalog Button");
  Serial.println("\nPress one or more buttons");
  
  // siapkan OLED
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED allocation failed");
    for (;;); // Don't proceed, loop forever
  }
  oled.setFont();
  oled.setTextColor(WHITE);
}

void loop() {

  // baca button 
  int bt_analog = _btn.aRead();
  byte bt_bin = _btn.a2d(bt_analog); // konversi ke digital
   
  // tampilkan ke OLED
  oled.clearDisplay();

  oled.setTextSize(1);
  oled.setCursor(0, 0);  // Start at top-left corne
  oled.printf("a= %d", bt_analog);

  oled.setTextSize(2);
  oled.setCursor(0, 12);
  oled.print("BTN=");
  oled.print(bt_bin, BIN);  
  oled.display();

  // tampilkan ke LED
  for (int i=0; i<N_LED; i++) {
    _led.write(i, (bt_bin>>i) & 1);
  }
  
  delay(200);
}
