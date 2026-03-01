/* Program : Tampilan Plotter   
* Contoh memrogram:    
* - oled grafik  
* - serial plotter  
* - membuat buffer geser  
* - operasi bitwise  
* (c) Eko M. Budi, 2023  
*/  
  
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
  
// rutin untuk plot biner ke oled 128x32 pixel  
// akan diplot 32 byte terakhir  
#define N_BYTE 32  
#define B_HEIGHT (D_HEIGHT/8)  
#define B_WIDTH (D_WIDTH/N_BYTE)  
  
void oledPlotBiner(byte d) {  
// buffer untuk menyimpan sebanyak N_BYTE terakhir   
static byte d_buffer[N_BYTE];  
  
// sebelum simpan data, geser memori buffer  
for (int i=0; i<N_BYTE-1; i++) {  
   d_buffer[i] = d_buffer[i+1];  
}  
// masukkan data terakhir  
d_buffer[N_BYTE-1] = d;  
   
oled.clearDisplay();  
  
// mulai plot semua d_buffer    
for (int j=0; j<N_BYTE-1; j++) {      
  int d0 = d_buffer[j];  
  int x = j*B_WIDTH;    // posisi x grafik  
  // plot semua bit      
  for (int i=0; i<8; i++) {        
      int y = i*B_HEIGHT; // posisi y grafik  
      int h = (d0 & 1); // ambil bit-0 yang akan diplot 
       d0 = d0 >> 1; // geser agar bit-0 berikutnya siap 
 
// sesuaikan h dan y, karena koordinat ke kiri bawah        
if (h) {  
        h = B_HEIGHT-1;                  
      }  
      else {  
        y = y + B_HEIGHT-1;  
        h = 1;          
      }  
      // gambar grafiknya  
      oled.fillRect(x, y, B_WIDTH, h, SSD1306_WHITE);  
    }  
  }  
  oled.display();  
}  
  
// Plot data bit per bit ke Serial Plotter  
void plotBiner(byte d) {    
  const int ROW_H = 2;  
  
 // plot 8 bit  
 // karena plot pertama ada di bawah    
 // maka plot dari bit-7 ke bit-0    
 for(int i=0; i<8; i++) {      
    int h = (d & 0x80) >> 7; // ambil bit-7      
    d = d << 1;  // geser agar bit-7 siap  
    Serial.print(h + (i*ROW_H));   
    Serial.print(' ');  
  }  
  Serial.println();  
}  
  
// data  
byte counter=0;  
  
void setup(void)  
{  
  Serial.begin(BAUD);  
  Serial.println("\nPlotter Test");  
  
  // siapkan OLED  
  if (!oled.begin(SSD1306_SWITCHCAPVCC, D_ADDR)) {  
    Serial.println("OLED allocation failed");      
    for (;;); // Don't proceed, loop forever  
  }  
  
  // Header untuk plot bit per bit  
  Serial.println("d7 d6 d5 d4 d3 d2 d1 d0");    
} 

void loop(void)  
{  
counter++;    
oledPlotBiner(counter);    
plotBiner(counter);    
delay(100);  
} 
