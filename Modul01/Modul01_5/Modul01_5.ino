/* Program : Running LED State Driven  
* Menyalakan 3 LED sesuai tombol  
* Tidak ditekan: jalan ke kiri/kanan  
* Ditekan: berhenti  
* Dilepas: ganti arah  
* Status LED, tombol, dan waktu tampil ke serial plotter   
* Belajar:  
*  - algoritma state-driven  
*  - event waktu  
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
  _t_last = _t_last + PERIODE;   return true;  
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
  
// putar LED ke kanan  
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
  
// Fungsi-fungsi state  
// Algoritma dasar, sesuai state:  
// - jika ada event, aksi, ganti state  
 
void stateKiri() {    
  if (_bt0) {      
    // tak ada aksi  
    _state = HENTI_KIRI; // ubah state  
  }    
  else if (_t_event) {      
    ledRotateLeft();  // aksi      
    ledWriteAll();      
    // tak ubah state  
  }  
}   
 
void stateHentiKiri() {    
  if (!_bt0) {      
    // tak ada aksi  
    _state = KANAN; // ubah state  
  }  
} 
 
void stateKanan() {  
  if (_bt0) {      
    // tak ada aksi  
    _state = HENTI_KANAN; // ubah state  
  }    
  else if (_t_event) {      
    ledRotateRight();  // aksi      
    ledWriteAll();      
    // tak ubah state  
  }  
}  
 
void stateHentiKanan() {  
  if (!_bt0) {      
    // tak ada aksi  
    _state = KIRI; // ubah state  
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
  Serial.println("\nRunning LED State Driven");  
    
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
  
// loop dengan algoritma state-driven  
void loop() {  
  // baca semua input (event)  
  _t_event = tIsTime();  
  _bt0 = digitalRead(BT0);  
  
  switch(_state) {  
    case HENTI_KANAN : stateHentiKanan(); break; 
    case HENTI_KIRI : stateHentiKiri(); break;      
    case KIRI : stateKiri(); break;      
    case KANAN : stateKanan(); break;      
  } 
    // kirim ke serial plotter    
  plotData();  
  delay(50);  
}  
