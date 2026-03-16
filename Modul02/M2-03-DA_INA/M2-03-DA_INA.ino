/* Program : DA dan INA
 * Contoh mengeluarkan tegangan dengan DA
 * dan membaca tegangan & arus dengan INA
 * diatur dengan button dan tampil di Serial Monitor / Tracer
 * Koneksikan AO0 ke V+; 
 *   V- ke resistor (min 330 ohm);
 *   resistor ke GND
 * Belajar:
 * - dacWrite
 * - Kelas INA219
 * (c) Eko M. Budi, 2023
 */

// pustaka
#include <TFScope22.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

#define BAUD 500000

#include <Adafruit_GFX.h>  
#include <Adafruit_SSD1306.h>  
  
// pesan obyek display  
#define D_WIDTH   128   
#define D_HEIGHT  32   
#define D_RESET   4  
#define D_ADDR    0x3C  
Adafruit_SSD1306 oled(D_WIDTH, D_HEIGHT, &Wire, D_RESET); 

// Pesan obyek Button
int tresholds[] = 
   {2036,2390,2587,2850,2961,3075,3275};
AnalogButton _btn(tresholds);

// pesan obyek INA219
Adafruit_INA219 _ina219;

void setup() {
  // Siapkan Serial
  Serial.begin(BAUD);
  Serial.println("\nDA-INA219 Test");
  Serial.println("DA  VI-(V)  Vsh(mV)  I(mA)  P(mW)");

  // siapkan OLED  
  if (!oled.begin(SSD1306_SWITCHCAPVCC, D_ADDR)) {      
     Serial.println("OLED allocation failed");      
     for (;;); // Don't proceed, loop forever  
  }  
  oled.setFont();  
  oled.setTextColor(WHITE); 

  // siapkan INA219
  if (!_ina219.begin()) {
    while (1) { delay(100); }
  }
}

void loop() {
  // variabel output, statis
  static byte da_out=0;

  // periksa button
  byte bt = _btn.read();

  // proses input button
  if (bt & BUTTON0) {
    if (da_out > 0) da_out--;
    delay(50);
  }
  else if (bt & BUTTON2) {
    if (da_out < 255) da_out++;
    delay(50);    
  }
  else {
    delay(1000);
  }
  // tulis output
  dacWrite(AO0, da_out);

  // baca INA
  float shunt_mv = _ina219.getShuntVoltage_mV();
  float bus_v = _ina219.getBusVoltage_V();
  float current_ma = _ina219.getCurrent_mA();
  float power_mw = _ina219.getPower_mW();

  // tulis hasil
  Serial.printf("%3d %6.2f %6.2f %6.2f %6.2f\n",
      da_out, bus_v, shunt_mv, current_ma, power_mw);  

   oled.clearDisplay();  
  
  // print desimal       
  oled.setTextSize(1);  
  oled.setCursor(0, 0);   
  oled.print("Vbus: "); 
  oled.print(bus_v);  
  
  oled.setCursor(0, 10); 
  oled.print("Vshunt: ");     
  oled.print(shunt_mv);  
  
  // print binary    
  oled.setTextSize(1);    
  oled.setCursor(0, 20); 
  oled.print("I: ");   
  oled.print(current_ma);  

  oled.setCursor(60, 20);     
  oled.print("P: ");
  oled.print(power_mw); 
   
  // keluarkan    
  oled.display(); 
}
