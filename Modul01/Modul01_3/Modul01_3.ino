/* Program : BASIC digital I/O  
* Contoh digital I/O  
* Menghitung counter sesuai penekanan tombol  
* dan mengubah nyala LED0 - LED2 sesuai counter  
* Kondisi tombol dan LED ditampilkan ke serial plotter  
* Belajar:* Kondisi tombol dan LED ditampilkan ke serial plotter  
* - tipe data bool, LOW, HIGH  
* - digitalMode, digitalRead, digitalWrite  
*   
* (c) Eko M. Budi, 2023  
*/  
#define BAUD 500000  
// Pin Standar kit ESCOPE 2020 ESP32  
// Pin Standar kit ESCOPE 2022 ESP32  
#define AO0 26  
#define AO1 25  
#define DO0 12  
#define DO1 14 
#define DO2 27 
#define DO3 13  
#define AI0 36  
#define AI1 39  
#define DI0 34  
#define DI1 35  
#define DI2 32  
#define DI3 17  
#define LED0 16  
#define LED1 4  
#define LED2 15  
#define BT0 33  
bool bt0;       
// data penekanan tombol  
int counter=0;  // data counter  
// Fungsi untuk plot data ke serial plotter  
void plotData() {    
// setiap data diplot 5 titik    
for(int i=0; i<5; i++) {      
int bt = (bt0 ? 5:4);  // operator pilih      
Serial.printf("%d %d\n", bt, counter);    
}  
}  
// Definisi pin rangkaian 4 LED  
void setup() {    
Serial.begin(BAUD);  
Serial.println("\nBasic DIO");  
Serial.println("T Counter");  
// Siapkan Output    
pinMode(LED0, OUTPUT);    
pinMode(LED1, OUTPUT);    
pinMode(LED2, OUTPUT);  
// Siapkan Input    
pinMode(BT0, INPUT);  
}  
void loop() {  
bt0 = digitalRead(BT0); // baca tombol 
// kalau ditekan 
if (bt0) { // naikkan counter antara 0-3  
counter = (counter + 1) % 4;    
// nyalakan LED sesuai counter  
switch (counter) {        
case 3 : digitalWrite(LED2, HIGH); break;        
case 2 : digitalWrite(LED1, HIGH); break;        
case 1 : digitalWrite(LED0, HIGH); break;        
case 0 :  
digitalWrite(LED0, LOW);          
digitalWrite(LED1, LOW);          
digitalWrite(LED2, LOW);  
}  
}  
plotData();  // tampilkan ke serial plotter  
delay(500); // delay   
}   
