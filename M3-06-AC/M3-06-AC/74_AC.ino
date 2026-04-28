void acPrintHeader() {
  Serial.println("F(Hz) Periode(ms) Gain(%)");
}

void acMeasure(unsigned prd) {
  periode_signal = prd;
  signal_type = ST_SINUS;

  trMeasure();

  int item_mv[N_SIGNAL + 1];
  int v0_min = 5000;
  int v0_max = -5000;
  int v1_min = 5000;
  int v1_max = -5000;

  while (!lbuffer.isEmpty()) {
    lbuffer.take(item_mv);
    int v0 = item_mv[1];
    int v1 = item_mv[2];

    if (v0 > v0_max) v0_max = v0;
    if (v0 < v0_min) v0_min = v0;
    if (v1 > v1_max) v1_max = v1;
    if (v1 < v1_min) v1_min = v1;
  }

  int v0_pp = v0_max - v0_min;
  int v1_pp = v1_max - v1_min;
  float freq = 1000.0 / prd;
  int gain_pct = (v0_pp > 0) ? (v1_pp * 100 / v0_pp) : 0;

  Serial.print(freq, 2);
  Serial.print(" ");
  Serial.print(prd);
  Serial.print(" ");
  Serial.println(gain_pct);
}

void acAction() {
  acPrintHeader();
  int periods[] = {10000, 5000, 2000, 1000, 500, 200, 100, 50};
  int n_periods = 8;
  for (int i = 0; i < n_periods; i++) {
    acMeasure(periods[i]);
  }
}
