/*
 * Pengukuran transien
 */

// banyak gelombang untuk warm-up
// coba nanti diubah jadi 2 / 3
#define N_WARMUP 2

// generate signal
void generateSignal() {
  int tp = p1.elapsed() % periode_signal;
  da_mv = fSignal(tp, periode_signal);
  dacWrite(p_da, mvToDa(da_mv));
}

// trace signal, masuk buffer
void traceSignal() {
  int item_mv[N_SIGNAL];
  item_mv[0]=da_mv;  // sinyal output 
  for(int i=0; i<N_AD; i++) {
     int ad = analogRead(p_ad[i]);
     item_mv[1+i] = adToMv(ad);
  }
  lbuffer.put(item_mv); // masuk buffer
}

// print sinyal dari buffer ke serial
void trPlotSignal() {
  char buff[40];
  int item_mv[N_SIGNAL];
  while (!lbuffer.isEmpty()) {
    lbuffer.take(item_mv);  
    sprintf(buff, "%4d %4d %4d",
      item_mv[0], item_mv[1], item_mv[2]);
    Serial.println(buff);
  }
}

// print header sinyal 
void trPlotHeader() {
  char buff[60]; 
  sprintf(buff, "Vout V0   V1  %s Periode=%d", 
    signal_strs[signal_type], periode_signal);
  Serial.println(buff);
}

// fungsi pengukuran transien
void trMeasure() {
  ts_signal = periode_signal / N_DATA;
  if (ts_signal < 1)
    ts_signal = 1;  
  p1.reset(ts_signal);
  lbuffer.reset();

  // pemanasan
  for (int i=0; i<N_DATA*N_WARMUP; i++) {
    p1.wait();
    generateSignal();
  }

  // pengukuran transien
  for (int i=0; i<N_DATA; i++) {
    p1.wait();
    generateSignal();
    traceSignal();
  }
}

// fungsi pengukuran transien
void trAction() {
  trPlotHeader();
  trMeasure();
  trPlotSignal();
}
