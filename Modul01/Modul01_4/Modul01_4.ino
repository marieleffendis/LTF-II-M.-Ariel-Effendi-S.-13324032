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
  
// menggunakan array untuk permudah I/O  
#define N_LED 3  
int pin_led[N_LED] = {LED0, LED1, LED2}; // pin  
bool d_led[N_LED]={1,0,0}; // status LED  
bool bt0;       // data penekanan tombol  
  
// putar bit ke kiri  
void rotateLeft() {    
   bool temp=d_led[0];    
   for(int i=1; i<N_LED; i++) {      
     d_led[i-1] = d_led[i];  
  }  
  d_led[N_LED-1] = temp;  
}  
 
// putar bit ke kanan  
void rotateRight() {
  bool temp=d_led[N_LED-1];
   for(int i=N_LED-1; i>0; i--) {
     d_led[i] = d_led[i-1];
  }
  d_led[0] = temp;
}
  
// Fungsi untuk plot data ke serial plotter 
void plotData() {    
  // setiap data diplot 5 titik    
  for(int i=0; i<5; i++) {      
    Serial.printf("%d %d %d %d\n",         
      bt0,        
      d_led[0] + 2,        
      d_led[1] + 4,        
      d_led[2] + 6);  
  }  
}  
  
// Mempersiapkan I/O  
void setup() {    
  Serial.begin(BAUD);  
  Serial.println("\nRunning LED");  
    
  // Siapkan mode I/O    
  // pakai array lebih ringkas    
  for (int i=0; i<N_LED; i++) {  
      pinMode(pin_led[i], OUTPUT);  
  }    
  pinMode(BT0, INPUT); 
  
  // Header Plot  
  Serial.println("d0 d1 d2 bt0");  
}  
  
// loop dengan algoritma multi-loop  
void loop() {  
  bt0=digitalRead(BT0);  
  
  // proses data sesuai event input  
  if (bt0) {  
    rotateRight();      
  }   else {  
    rotateLeft();  
  }    
  
  // tulis semua output    
  for(int i=0; i<N_LED; i++) {  
    digitalWrite(pin_led[i], d_led[i]);  
  }    
  plotData();    
  delay(200);  
} 
