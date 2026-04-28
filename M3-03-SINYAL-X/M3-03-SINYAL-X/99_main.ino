/*
 * Program utama
 * Algoritma multi-loop periodik
 */

// data
#define N_DATA 500
#define N_SIGNAL ST_MAX+1
int signals[N_SIGNAL];

// periode sinyal
unsigned periode_signal = 5000;  // periode sinyal dalam ms

// generator
unsigned n_generator = 100;        // banyak data/gelombang
unsigned ts_generator = periode_signal/n_generator;

// time sampling
unsigned ts_tracer = periode_signal/N_DATA;

// timer untuk 2 task +
Periodic p1(ts_generator);  
Periodic p2(ts_tracer); 

// task membangkitkan sinyal berbagai gelombang
void generateSignal() {
  // hitung waktu dari periodic
  unsigned t = p1.elapsed() % periode_signal;
  
  signals[0] = fSin(t, periode_signal);  
  signals[1] = fRectangle(t, periode_signal);
  signals[2] = fTriangle(t, periode_signal);
  signals[3] = fSawTooth(t, periode_signal);
  signals[4] = fCenter(fNoise(), 10);
}

// task plot semua sinyal
void plotSignal() {
  char buff[40];
  sprintf(buff, "%4d %4d %4d %4d %4d",
    signals[0], signals[1], signals[2], signals[3], signals[4]);
  Serial.println(buff);
}

// plot header
void plotHeader() {
  char buff[80]; 
  sprintf(buff, "Vsin Vrec Vtri Vsaw Vnoise N_gen=%d Ts_gen=%d",
    n_generator, ts_generator);  
  Serial.println(buff);
}

void setup() {
  Serial.begin(BAUD);    
  delay(3000);
  Serial.println();
  plotHeader();
}

void loop() {
  if (p1.isTime()) {
    generateSignal();
  }
  else if (p2.isTime()) {
    plotSignal();
  }
  else if (Serial.available()) {
    // terima banyak data baru
    int n_gen = Serial.parseInt();
    if ((n_gen >= 10) && (n_gen <= N_DATA)) {
      // sesuaikan timer generator
      n_generator = n_gen;
      ts_generator = periode_signal / n_gen;
      p1.reset(ts_generator);
      plotHeader();
    }
  }
}
