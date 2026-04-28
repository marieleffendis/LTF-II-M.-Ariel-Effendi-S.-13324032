/*
 * Program utama 
 * - task generateSignal menghasilkan sinyal sinus ke DA
 *   dengan periode tertentu, waktu sampling sekecil mungkin
 * - task traceSignal mengukur dan mengirim ke serial
 *   dengan waktu sampling yang bisa diubah
 *   
 * Coba jalankan dan amati luaran dengan Serial Plotter,
 * ubah waktu sampling dari 100 ms, turun sekecil-kecilnya
 */ 

// Rentang waktu sampling
// sesuaikan dengan waktu eksekusi 
// AD, DA dan Serial
#define TS_MIN 1
#define TS_MAX 100

// banyak data sesuai Serial Plotter
#define N_DATA 500

// Banyak sampling tracer yang akan dirata-rata
// turunkan jika tak mengejar waktu samplingnya
#define N_SUM 5

// Signal tracer 2 kanal
#define N_AD 2
int in_mv[N_AD];

int out_mv;
int p_out = AO0;

int type_signal = ST_SINUS;
unsigned periode_signal = 1000;  // periode dalam ms

// pin 
int p_in[N_AD] = {AI0, AI1};

// time sampling
// timer untuk tracer, masih pakai periodic mS
// Coba jajagi kalau pakai uS; apakah layak ?
unsigned ts_tracer = TS_MAX;
Periodic p2(ts_tracer);  

// Baca signal dengan rata-rata
void traceSignal() {
  long sum[N_AD];
  for (int i=0; i<N_AD; i++) {
    sum[i]=0;
  }  
  for (int j=0; j<N_SUM; j++) {
    for (int i=0; i<N_AD; i++) {
      sum[i] += analogRead(p_in[i]);
    }
  }
  for (int i=0; i<N_AD; i++) {
    in_mv[i] = adToMv(sum[i]/N_SUM);
  }  
}

void plotSignal() {
  char buff[40];
  sprintf(buff, "%4d %4d %4d",
    out_mv, in_mv[0], in_mv[1]);
  Serial.println(buff);
}

void plotHeader() {
  char buff[60]; 
  sprintf(buff, "Vout V0   V1  Ts_tracer=%d P_signal=%d",
    ts_tracer, periode_signal);  
  Serial.println(buff);
}

//--------------------------------------------
// Bagian signal generator


// timer untug signal generator
// memakai micros agar teliti
unsigned ts_generator = TS_MIN*1000/2;  // setengah TS_MIN, dalam uS
PeriodicMicros p1(ts_generator);  

// menghasilkan sinyal secare periodik
void generateSignal() {
  unsigned long p = periode_signal*1000;
  unsigned long t = p1.elapsed() % p;
  out_mv = fSignal(type_signal, t, p);
  dacWrite(p_out, mvToDa(out_mv));
}

//-----------------------------------------
// Utama, algoritma multi-loop periodik
void setup() {
  Serial.begin(500000);    
  delay(3000);
  Serial.println();
  Serial.println(PROG_NAME);
  
  plotHeader();
}

void loop() {
  if (p1.isTime()) {
    generateSignal();    
  }
  else if (p2.isTime()) {
    traceSignal();
    plotSignal();
  }
  else if (Serial.available()) {
    // terima waktu sampling baru
    unsigned ts = Serial.parseInt();
    if ((ts >= TS_MIN) && (ts <= TS_MAX)) {
      ts_tracer = ts;
      p2.reset(ts_tracer);
      plotHeader();
    }
  }
}
