#include <Stepper.h> 
 
// Pin drive motor ULN2003 
#define IN1 13 
#define IN2 12 
#define IN3 14 
#define IN4 27 

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

float stepsPerRevolution = 2048;  // jumlah langkah per satu putaran penuh 
float revolution = 360; // 1 revolusi penuh 360 derajat 
float step_angle = revolution / stepsPerRevolution; // step angle 
 
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
int currenttime;

void tReset() {    
  _t_last = millis();  
}   
 
boolean tIsTime() {  
  if ((millis()-_t_last) < PERIODE) return false;  
  _t_last = millis(); return true;  
}  
  
bool _bt0;

enum {RESET, START, STOP};  
int _state = RESET;  
int i = 1;

void step_Degree(int degree){ 
  myStepper.step(degree/step_angle); 
} 
  
void stateStop() {
  if (currenttime > 270 || _bt0) {
    _state = RESET; 
    _t_now = millis();
  }  
  if (_bt0) {
    _state = START; 
    _t_now = millis();
  }
}   
 
void stateStart() {
  if (currenttime <= 270){
    for (int i; i <= 270; i++) {
        step_Degree(1); 
    }
    currenttime++;
  }
  else if (currenttime > 270 || _bt0) {
    _state = RESET; 
    _t_now = millis();
  }  
  else if (_bt0) {
    _state = STOP; 
    _t_now = millis();
  }
} 

void stateReset() {
  step_Degree(0);
  if (_bt0) {      
    _state = START; 
    _t_now = millis();
  }   
}  
  
void plotData() {  
  Serial.println(currenttime);
  Serial.println(millis());
  Serial.println(_t_last);
  Serial.println(_t_now);
}  
  
void setup() {   
  Serial.begin(BAUD);  
  pinMode(BT0, INPUT_PULLUP);   
  tReset();  
  _t_now = millis();
  myStepper.setSpeed(15); 
}  
  
void loop() {  
  _t_event = tIsTime();  
  _bt0 = digitalRead(BT0);  
  
  switch(_state) {  
    case STOP : stateStop(); break;      
    case RESET : stateReset(); break;      
    case START : stateStart(); break;      
  }  

  if (_t_event) {
    plotData();  
  }
  delay(50);  
}
