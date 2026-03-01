/* Program : Running LED Event Driven  
* Menyalakan 3 LED sesuai tombol  
* Tidak ditekan: jalan ke kiri/kanan  
* Ditekan: berhenti  
* Dilepas: ganti arah  
* Status LED, tombol, dan waktu tampil ke serial plotter 
* Belajar:  
* - algoritma event-driven  
* - event waktu  
*   
* (c) Eko M. Budi, 2023  
*/  
 
#define BAUD 500000  
  
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
  
  
// Pewaktuan --------------------------------  
// event periodik 500 ms  
#define PERIODE 500  
long _t_last;     // waktu aksi sebelumnya  
bool _t_event;    // event waktu  
  
// memulai event waktu periodik  
void tReset() {    
  _t_last = millis();  
}   
 
boolean tIsTime() {  
  if ((millis()-_t_last) < PERIODE) return false;  
  _t_last = _t_last + PERIODE;    
  return true;  
}  
  
// I/O Tombol -----------------------------------  
bool _bt0;       // data global penekanan tombol  
  
  
// I/O LED 3 buah ------------------------------  
// menggunakan array untuk permudah  
#define N_LED 3  
  
// data global LED  
int _led_pins[N_LED]={LED0, LED1, LED2};  
bool _led_status[N_LED]={1,0,0};  
  
// output semua LED sesuai status  
void ledWriteAll() {    
  for(int i=0; i<N_LED; i++) {  
    digitalWrite(_led_pins[i], _led_status[i]);  
  }  
}  
  
// putar LED ke kiri  
void ledRotateLeft() {    
  bool temp=_led_status[0];    
  for(int i=0; i<N_LED-1; i++) {      
    _led_status[i] = _led_status[i+1];  
  }  
  _led_status[N_LED-1] = temp;  
}  
  
void ledRotateRight() {
  bool temp=_led_status[N_LED-1];
   for(int i=N_LED-1; i>0; i--) {
     _led_status[i] = _led_status[i-1];
  }
  _led_status[0] = temp;
}
  
// FSA -------------------------------------  
// definisi state, bisa pakai konstanta  
// #KIRI 0  
// #HENTI_KIRI 1  
// dst  
// namun bahasa C punya cara lebih singkat pakai enum  
enum {KIRI, HENTI_KIRI, KANAN, HENTI_KANAN};  
int _state = KIRI;  
  
// Fungsi-fungsi event  
// Algoritma dasar, sesuai event:  
// - periksa state, aksi, ganti state  
  
void eventT() {   switch(_state) {  
    case KIRI : ledRotateLeft(); ledWriteAll(); break;              
    case KANAN : ledRotateRight(); ledWriteAll(); break; // betulkan !  
  }  
}   
void eventBt0High() {   switch(_state) {  
    case KIRI : _state = HENTI_KIRI; break;     
    case KANAN : _state = HENTI_KANAN; break;  // betulkan !  
  }  
}   
void eventBt0Low() {    
  switch(_state) {  
    case HENTI_KIRI: _state = KANAN; break;      
    case HENTI_KANAN: _state = KIRI; break; // betulkan !  
  }  
}  
  
// Serial plotter -----------------------------  
// Fungsi untuk plot data ke serial plotter  
void plotData() {  
  // print header dengan waktu  
  Serial.printf("d0 d1 d2 bt0 state T=%lu\n", millis()); 
  // print data  
  Serial.printf("%d %d %d %d %d\n",   
      _led_status[0],  
      _led_status[1] + 2,  
      _led_status[2] + 4,  
      _bt0 + 6, 
       _state + 8);  
}  
  
// UTAMA ------------------------------------  
void setup() {    
  Serial.begin(BAUD);  
  Serial.println("\nRunning LED Event Driven");  
    
  // Siapkan mode I/O    
  pinMode(BT0, INPUT);    
  for (int i=0; i<N_LED; i++) {     
    pinMode(_led_pins[i], OUTPUT);  
  }  
  
  // siapkan waktu    
  tReset();  
  
  // Header Plot  
  Serial.println("d0 d1 d2 bt0 state");  
}  
  
// loop dengan algoritma event-driven  
void loop() {  
  // baca semua input (event)  
  _t_event = tIsTime();  
  _bt0 = digitalRead(BT0);  
  
  // jalankan respon sesuai event    
  if (_t_event) {      
    eventT();      
  }    
  else if (_bt0 == HIGH) {      
    eventBt0High();      
  }   
  else if (_bt0 == LOW) {      
    eventBt0Low();      
  }  
  
  // kirim ke serial plotter    
  plotData();  
  delay(50);  
}
