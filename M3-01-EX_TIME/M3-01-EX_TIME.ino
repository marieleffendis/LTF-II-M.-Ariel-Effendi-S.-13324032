/* Program : Execution Time
 * Menggunakan millis dan micros 
 * untuk mengukur waktu eksekusi  
 * (c) Eko M. Budi, 2022
 */

// BAUD serial
// coba ubah ke 115200, 9600
#define BAUD 9600
#define PROG_NAME "Ukur waktu eksekusi"

// Pin Standar kit ESCOPE 2020
#define DO0 13
#define DO1 12
#define DO2 14
#define DO3 27
#define DO4 33

#define DI0 32
#define DI1 35
#define DI2 34
#define DI3 39
#define DI4 36

#define AO0 26
#define AO1 25

#define LED0 5
#define BT0 4
#define BT1 16
#define BT2 17

#define N_TEST 10000
unsigned long ms_start, ms_finish;
unsigned long us_start, us_finish;

void printData(char *test){
  char buff[40];
  sprintf(buff, "%-12s %6lu %8lu", test,
    ms_finish-ms_start,
    us_finish-us_start);    
  Serial.println(buff);
}

void setup() {
  Serial.begin(BAUD);
  delay(1000);
  Serial.println();
  Serial.println(PROG_NAME);
}

void loop() {
  Serial.print("Test serial");
  // Test mengukur kirim data serial
  ms_start=millis();
  us_start=micros();
  for(int i=0; i< N_TEST; i++) {
     Serial.print(".");
  }
  us_finish=micros();
  ms_finish=millis();
  
  Serial.println();
  Serial.print("N_TEST=");
  Serial.println(N_TEST);
  Serial.println("Test             mS       uS");

  printData("print()");

  // Test mengukur delay
  ms_start=millis();
  us_start=micros();
  delay(N_TEST);
  us_finish=micros();
  ms_finish=millis();
  printData("delay(N)");
  
  // Test mengukur delay
  ms_start=millis();
  us_start=micros();
  delayMicroseconds(N_TEST);  
  us_finish=micros();
  ms_finish=millis();
  printData("delayUS(N)");


  // Test mengukur delay
  ms_start=millis();
  us_start=micros();
  for(int i=0; i< N_TEST; i++) {
    long a = 10000;
    long b = 30000;
    long c = 20000;
    long d = a * b / c;
  }
  us_finish=micros();
  ms_finish=millis();
  printData("long calc");

  // Test mengukur delay
  ms_start=millis();
  us_start=micros();
  for(int i=0; i< N_TEST; i++) {
    double a = 10000;
    double b = 30000;
    double c = 20000;
    double d = a * b / c;
  }
  us_finish=micros();
  ms_finish=millis();
  printData("double calc");

  // Test mengukur delay
  ms_start=millis();
  us_start=micros();
  for (int i=0; i<N_TEST; i++) {
    digitalWrite(DO0, i & 1);
  }
  us_finish=micros();
  ms_finish=millis();
  printData("digitalWrite");

  // Test mengukur delay
  ms_start=millis();
  us_start=micros();
  for (int i=0; i<N_TEST; i++) {
    digitalRead(DI0);
  }
  us_finish=micros();
  ms_finish=millis();
  printData("digitalRead");

  // Test mengukur delay
  ms_start=millis();
  us_start=micros();
  for (int i=0; i<N_TEST; i++) {
    dacWrite(AO0, i);
  }
  us_finish=micros();
  ms_finish=millis();
  printData("dacWrite");

  // Test mengukur delay
  ms_start=millis();
  us_start=micros();
  for (int i=0; i<N_TEST; i++) {
    analogRead(DI0);
  }
  us_finish=micros();
  ms_finish=millis();
  printData("analogRead");

  Serial.println("-------------------");
  delay(10000);
  
  /* cobalah mengukur lama eksekusi 
   * perintah membaca INA
   *   getShuntVoltage_mV();
   *   getBusVoltage_V();
   *   getCurrent_mA();
   *   getPower_mW();
   */
}
