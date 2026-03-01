/* Program : Tampilan OLED  
* Contoh memrogram OLED dan Serial  
* Menampilkan angka desimal, hex dan biner  
* (c) Eko M. Budi, 2023  
 */  
  
// pustaka I2C  
// menyediakan obyek Wire   
#include <Wire.h>  
  
#define BAUD 500000  
  
// pustaka OLED  
#include <Adafruit_GFX.h>  
#include <Adafruit_SSD1306.h>  
  
// pesan obyek display  
#define D_WIDTH   128   
#define D_HEIGHT  32   
#define D_RESET   4  
#define D_ADDR    0x3C  
Adafruit_SSD1306 oled(D_WIDTH, D_HEIGHT, &Wire, D_RESET);  
  
void oledPrintByte(char d) {    
  oled.clearDisplay();  
  
  // print desimal       
  oled.setTextSize(1);  
  oled.setCursor(0, 0);  // Start at top-left corne    
  oled.print(d);  
  
  oled.setCursor(40, 0);      
  oled.print(d, HEX);  
  
  // print binary    
  oled.setTextSize(2);    
  oled.setCursor(0, 12);    
  oled.print(d, BIN);  
  
  // keluarkan    
  oled.display();  
} 
void printByte(char d) {  
  Serial.printf("%c %d %x ", d, d, d);  
  Serial.println(d, BIN);    
}  
  
byte counter=0;  
  
void setup(void)  
{  
  // siapkan Serial  
  Serial.begin(BAUD);  
  Serial.println("\nOLED Test");  
  
  // siapkan OLED  
  if (!oled.begin(SSD1306_SWITCHCAPVCC, D_ADDR)) {      
     Serial.println("OLED allocation failed");      
     for (;;); // Don't proceed, loop forever  
  }  
  oled.setFont();  
  oled.setTextColor(WHITE);  
}   
 
void loop(void)  
{  
  counter++;  
   
  oledPrintByte(counter);    
  printByte(counter);  
    
  delay(100);  
}  
