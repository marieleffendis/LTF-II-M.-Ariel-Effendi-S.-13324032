/* Program : Ring Buffer
 * Mencoba ring buffer 
 * Membangkitkan N_COL sinyal dengan periode PRD.
 * masuk ke ringbuffer dengan kapasitas N_ROW.
 * Print sinyal dan moving average-nya.
 * 
 * Jalankan program ini amati luarannya pada Serial Monitor.
 * Perbaiki rutin movAverage agar sanggup menghitung MovAverage
 * dari RingBuffer sesuai window yang diminta.
 * 
 * (c) Eko M. Budi, 2022
 */

#include <TFRingBuffer.h>

// dimensi buffer
#define N_ROW 4
#define N_COL 2

// tipe data buffer
typedef struct {
  int signal[N_COL];
} DataType;

// Pesan ring buffer
RingBuffer<N_ROW, DataType> rbuff;

#define SIGNAL_AMP 10
#define SIGNAL_PRD 10

/* membuat sinyal, masuk ke buffer
 */
void generateSignal(unsigned t, unsigned prd) {
  DataType d;
  t = t % prd;
  
  // buat sinyal segitiga dan kotak
  d.signal[0] = t * SIGNAL_AMP / prd;
  d.signal[1] = (t < (prd/2) ? SIGNAL_AMP: 0);

  // masukkan ke buffer
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

/* plot sinyal dua kanal
 * masing-masing N_ROW data terakhir
 */
void plotSignal() {
  DataType d[N_ROW];
  char str[20];

  // berbagai contoh mengambil sinyal terakhir
  // pakai fungsi getLast(), paling terakhir
  rbuff.getLast(d[0]);

  // pakai fungsi getLast dgn index
  rbuff.getLast(1, d[1]);

  // pakai operator [], lebih cepat namun tanpa pengaman
  // akan dapat data garbage kalau idx tak valid !
  // untuk ambil dari depan, pakai index positif (0=first)
  // untuk ambil dari belakang, pakai index negatif (-1=last)
  d[2] = rbuff[-3];

  // pakai fungsi first()
  // index harus hitung dr last
  unsigned idx = rbuff.count()-4;
  rbuff.getFirst(idx, d[3]);

  // cetak
  for (int i=0; i<N_COL; i++) {
    for (int j=0; j<N_ROW; j++) {
      sprintf(str, "%02d  ", d[j].signal[i]);
      Serial.print(str);
    }
    sprintf(str, "%05.2f  ", movAverage(i,4));
    Serial.print(str);
  }
  Serial.println();
}

void plotHeader() {
  Serial.println("S00 S01 S02 S03 S0A     S10 S11 S12 S13 S1A");  
}

void setup() {
  char buff[100];
  Serial.begin(500000);
  delay(1000);
  Serial.println("Ring Buffer");
  plotHeader();
}

unsigned tick=0;
void loop() {
  generateSignal(tick++, SIGNAL_PRD);
  plotSignal();
  delay(1000);
}
