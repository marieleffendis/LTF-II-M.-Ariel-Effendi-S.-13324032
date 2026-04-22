/* Program : Moving Average
 * Mencoba moving average pakai ring buffer.
 * 
 * Editlah konstanta WINDOW_1 .. sampai WINDOW_3
 * Sampai didapat harga window yang tepat.
 * 
 * (c) Eko M. Budi, 2022
 */

#include <TFRingBuffer.h>

//--------------------------------
// dimensi buffer
#define N_ROW 40
#define N_COL 3

// tipe data buffer
typedef struct {
  int signal[N_COL];
} DataType;

// Pesan ring buffer
RingBuffer<N_ROW, DataType> rbuff;


//--------------------------------
// Sinyal
#define SIGNAL_AMP  100
#define SIGNAL_PRD  400

/* membuat sinyal, masuk ke buffer
 */
void generateSignal(unsigned t, unsigned prd) {
  DataType d;
  t = t % prd;
  
  // sinyal kotak
  d.signal[0] = (t > (prd/2) ? SIGNAL_AMP/2 : -SIGNAL_AMP/2);
  // sinus 3 frekuensi
  d.signal[1] = (int16_t)
    ((SIGNAL_AMP * sin(2*M_PI*t/prd))+
     (SIGNAL_AMP/5 * sin(2*M_PI*t/(prd/10)))+
     (SIGNAL_AMP/10 * sin(2*M_PI*t/(prd/100))));
  // sinus 1 frekuensi
  d.signal[2] = (int16_t)(SIGNAL_AMP * sin(2*M_PI*t/prd));
   
  rbuff.put(d);  
}

// menghitung moving average dari buffer 
// untuk kolom col dan window tertentu
float movAverage(int idx_s, int window) {
  float sum = 0.0;
  int count = 0;
  int available = rbuff.count();
  
  int actualWindow = (window < available) ? window : available;
  if (actualWindow <= 0) return 0.0;

  for (int i = 1; i <= actualWindow; i++) {
    sum += rbuff[-i].signal[idx_s];
    count++;
  }

  // Kembalikan nilai rata-rata
  return sum / (float)count;
}


// print 1 angka float
void printF(float f) {
  char str[20];
  sprintf(str, "%5.1f  ", f);
  Serial.print(str);
}

/* plot sinyal 
 *  Sn = sinyal asli
 *  A1 = movAverage WINDOW_1
 *  A2 = movAverage WINDOW_2
 *  A3 = movAverage WINDOW_3
 *  A4 = movAverage WINDOW_4
 */
// 
#define WINDOW_1   5
#define WINDOW_2   10
#define WINDOW_3   20
#define WINDOW_4   30

void plotSignal() {
  DataType d;
  // plot sinyal 0 dan 1
  for (int i=0; i<N_COL-1; i++) {
    printF(rbuff[-1].signal[i]);
    printF(movAverage(i, WINDOW_1));
    printF(movAverage(i, WINDOW_2));
    printF(movAverage(i, WINDOW_3));
    printF(movAverage(i, WINDOW_4));
  }
  // plot sinyal ke-2
  printF(rbuff[-1].signal[2]);
  Serial.println();    
}

void plotHeader() {
  Serial.println("S0    A01   A02   A03   A04   S1    A11   A12   A13   A14   S2");  
}

void setup() {
  char buff[100];
  Serial.begin(500000);
  delay(1000);
  Serial.println();
  Serial.println("Moving Average");
}

unsigned tick = 0;
void loop() {
  if ((tick % SIGNAL_PRD) == 0)
    plotHeader();
  generateSignal(tick, SIGNAL_PRD);
  plotSignal();
  delay(10);
  tick++;
}
