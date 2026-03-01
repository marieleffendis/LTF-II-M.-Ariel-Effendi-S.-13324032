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
  
#define PERIODE 500  
long _t_last;     
bool _t_event;
long _t_now = 0; 
  
void tReset() {    
  _t_last = millis();  
}   
 
boolean tIsTime() {  
  if ((millis()-_t_last) < PERIODE) return false;  
  _t_last = millis(); return true;  
}  
  
bool _bt0;   
  
#define N_LED 3  
int _led_pins[N_LED]={LED0, LED1, LED2};  
bool _led_status[N_LED]={0,0,0}; 
 
void ledWriteAll() { 
   for(int i=0; i<N_LED; i++) {  
     digitalWrite(_led_pins[i], _led_status[i]);  
  }  
}  
  
enum {JALAN, SIAP_JALAN, BERHENTI, SIAP_HENTI};  
int _state = JALAN;  
int i = 1;

void stateBerhenti() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED0, HIGH);   
  if (millis() - _t_now >= 30000) {
    _state = SIAP_JALAN;      
    _t_now = millis();
  }
}   
 
void stateSiapHenti() {   
  if (!_bt0 || _bt0) {      
    digitalWrite(LED2, LOW);
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
    if (millis() - _t_now >= 10000){
      _state = BERHENTI; 
      _t_now = millis();
    }
  }  
} 

void stateJalan() {
  digitalWrite(LED2, HIGH);
  if (millis() - _t_now < 120000) {
      if (_bt0){}
      else if (!_bt0){}
  }
  else if (millis() - _t_now >= 120000){
    if (_bt0) {      
      _state = SIAP_HENTI; 
      _t_now = millis();
    }    
    else if (!_bt0) { 
      digitalWrite(LED2, HIGH);
    }  
  } 
}  
 
void stateSiapJalan() {  
  digitalWrite(LED0, HIGH);
  delay(100);
  digitalWrite(LED0, LOW);
  delay(100);
  if (millis() - _t_now >= 10000) {      
      _state = JALAN;
      _t_now = millis();
      i++;
  }  
}  
  
void plotData() {  
  _led_status[0] = digitalRead(LED0);
  _led_status[1] = digitalRead(LED1);
  _led_status[2] = digitalRead(LED2);
  
  Serial.printf("%d %d %d %d %d\n",   
      _led_status[0],  
      _led_status[1] + 2,  
      _led_status[2] + 4,  
      _bt0 + 6,  
      _state + 8);  

  Serial.println(millis());
  Serial.println(_t_last);
  Serial.println(_t_now);
}  
  
void setup() {   
  Serial.begin(BAUD);  
  pinMode(BT0, INPUT_PULLUP);   
  for (int i=0; i<N_LED; i++) {      
    pinMode(_led_pins[i], OUTPUT);  
  }  
  tReset();  
  _t_now = millis();
}  
  
void loop() {  
  _t_event = tIsTime();  
  _bt0 = digitalRead(BT0);  
  
  switch(_state) {  
    case SIAP_HENTI : stateSiapHenti(); break; 
    case SIAP_JALAN : stateSiapJalan(); break;      
    case BERHENTI : stateBerhenti(); break;      
    case JALAN : stateJalan(); break;      
  }  

  if (_t_event) {
    plotData();  
  }
  delay(50);  
}
