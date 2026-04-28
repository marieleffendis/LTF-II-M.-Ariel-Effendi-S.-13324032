/* Program : Periodic
 * Menggunakan class periodic
 * untuk analisis jalannya task
 * 
 * (c) Eko M. Budi, 2022
 */
#include <TFScope22.h>
#include <TFPeriodic.h>

// konstanta simulasi waktu eksekusi
#define DELAY_1  2
#define DELAY_2  3
#define DELAY_3  5

unsigned delay3 = DELAY_3;

// Task-1,Task-2 memakai PeriodicMicros (uS)
PeriodicMicros p1(20000);
PeriodicMicros p2(30000);

// Task-3 memakai Periodic (mS)
Periodic p3(50);

// untuk analisis
#define N_RUN 200
#define N_TASK 3

unsigned n_run = 0;
double sum_tx = 0;
unsigned long sum_total = 0;

unsigned n_runt[N_TASK];
double sum_ttt[N_TASK];
double sum_txt[N_TASK];

void printHeader() {
  Serial.println("  Telapsed Ttardy   Texec Task");      

  // reset analisis
  n_run = 0;
  sum_tx = 0;
  sum_total = 0;
  for (int i=0; i<N_TASK; i++) {
    n_runt[i] = 0;
    sum_ttt[i] = 0;
    sum_txt[i] = 0;
  }
}

void printTask(int task, unsigned long tr, unsigned long tt, unsigned long tf) {
  char buff[60];
  unsigned long tx = tf-tt;
  sprintf(buff, "%10lu %6lu %6lu T%d", tr, tt, tx, task);
  Serial.println(buff);

  // perhitungan analisis
  n_run++;
  sum_tx += tx;

  n_runt[task]++;
  sum_ttt[task] +=tt;
  sum_txt[task] +=tx;  
}

void printAnalisis() {
  char buff[100];
  // Print analisis
  Serial.println("------------------------");
  Serial.println("N-Run    T-Run   U(%) Nr0 Nr1 Nr2    Tt0    Tt1    Tt2      Tx0      Tx1      Tx2");  
  sprintf(buff, "%5d %8lu %6.2f %3d %3d %3d %6.1f %6.1f %6.1f %8.1f %8.1f %8.1f",
    n_run, sum_total,
    (float) 100*sum_tx/sum_total,
    n_runt[0],
    n_runt[1],
    n_runt[2],
    (float) sum_ttt[0]/n_runt[0],
    (float) sum_ttt[1]/n_runt[1],
    (float) sum_ttt[2]/n_runt[2],    
    (float) sum_txt[0]/n_runt[0],
    (float) sum_txt[1]/n_runt[1],
    (float) sum_txt[2]/n_runt[2]);
  Serial.println(buff);    
  Serial.println();  
}

// loop task periodik
void loopTask() {
  unsigned long ttardy, trun, texec;
  for(;;) {
    if (p1.isTime()) {
      trun = p1.elapsed();
      ttardy = p1.tardy();
      delay(DELAY_1); // simulasi waktu eksekusi
      printTask(0, trun, ttardy, p1.tardy());
      return;
    }
    else if (p2.isTime()) {
      trun = p2.elapsed();
      ttardy = p2.tardy(); 
      delay(DELAY_2); // simulasi waktu eksekusi
      printTask(1, trun, ttardy, p2.tardy());
      return;
    }
    else if (p3.isTime()) {
      trun = p3.elapsed()*1000;// konversi ke uS
      ttardy = p3.tardy()*1000;
      texec = micros();
      delay(delay3); // simulasi waktu eksekusi
      printTask(2, trun, ttardy, micros()-texec);
      return;
    }
  }
}

void setup() {
  Serial.begin(500000);    
  Serial.println();
  Serial.println("Periodic Test");
}

void loop() {
  printHeader();

  // siapkan periodic manual
  p1.reset();
  p2.reset();
  p3.reset();

  // loop seluruh task periodik
  do {
    loopTask();
  } while ((n_run % N_RUN) > 0);
  sum_total = p1.elapsed();
  
  printAnalisis();

  // kalau ada, terima masukan delay_3
  while(Serial.available()) Serial.read();
  Serial.println("Masukkan Tx2 (ms):");
  for(;;) {
    if (Serial.available()) {
      unsigned d3 = Serial.parseInt();
      if (d3 >= 1) {
        delay3 = d3;    
        break;
      }
    }
  }
}
