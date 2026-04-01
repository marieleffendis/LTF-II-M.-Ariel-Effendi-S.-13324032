/*
 * Fungsi-fungsi untuk penghasil sinyal
 */

// enumerasi bentuk sinyal
enum {ST_NOISE, ST_SINUS, ST_RECTANGLE, ST_TRIANGLE, ST_SAWTOOTH, ST_MAX};

// string nama sinyal
char* signal_strs[] = {
  "NSE",
  "SIN",
  "REC",
  "TRI",
  "SAW"
};

// rentang signal (dalam mv)
int signal_min = 500;
int signal_max = 2500;
int signal_span = signal_max - signal_min;
int signal_type = ST_SINUS;

void setRange(int min, int max) {
  signal_min = min;
  signal_max = max;
  signal_span = signal_max - signal_min;
}

// fungsi menghasilkan sinyal sinus
// amplitudo signal_min - signal_max
int fSin(unsigned long t, unsigned long periode) {
  float s = sin(2*M_PI*t/periode);
  return (int)((s + 1)/2*signal_span)+signal_min;
}

// fungsi menghasilkan sinyal kotak
// rentang signal_min - signal_max
// fungsi menghasilkan sinyal sinus
int fSin(unsigned long t, unsigned long periode) {
  float s = sin(2.0 * PI * t / periode);
  return (int)((s + 1.0) / 2.0 * signal_span) + signal_min;
}

// fungsi menghasilkan sinyal kotak
int fRectangle(unsigned long t, unsigned long periode) {
  if ((t % periode) < (periode / 2)) {
    return signal_max;
  } else {
    return signal_min;
  }
}

// fungsi menghasilkan sinyal segitiga
// fungsi menghasilkan sinyal segitiga yang sinkron dengan kotak
int fTriangle(unsigned long t, unsigned long periode) {
  unsigned long phase = t % periode;
  unsigned long half_per = periode / 2;
  int value;

  if (phase < half_per) {
    // Fase NAIK: dari min ke max (durasi T/2)
    // Gunakan 2L dan casting long supaya tidak overflow saat dikali
    value = signal_min + (int)(2L * signal_span * phase / periode);
  } else {
    // Fase TURUN: dari max ke min (durasi T/2)
    // Kita balik logikanya: Max dikurangi kenaikan dari sisa waktu
    unsigned long phase_down = phase - half_per;
    value = signal_max - (int)(2L * signal_span * phase_down / periode);
  }
  
  return value;
}

// fungsi menghasilkan gigi gergaji
int fSawTooth(unsigned long t, unsigned long periode) {
  unsigned long phase = t % periode;
  return signal_min + (1L * signal_span * phase / periode);
}

// fungsi menghasilkan noise
// rentang signal_min - signal_max
int fNoise() {
  return random(signal_min, signal_max+1);
}

// fungsi menghasilkan sinyal dengan type tertentu
int fSignal(unsigned long t, unsigned long periode) {
  switch(signal_type) {
    case ST_SINUS: return fSin(t, periode); 
    case ST_RECTANGLE: return fRectangle(t, periode);
    case ST_TRIANGLE: return fTriangle(t, periode);
    case ST_SAWTOOTH: return fSawTooth(t, periode);
  }
  return fNoise();
}

// fungsi untuk gain (dalam persen) dan offset
int fAdjust(int s, int gain, int offset) {
  int s1 = s - offset;
  return (s1 * gain / 100) + offset;
}

// fungsi menaruh sinyal ke tengah dan diberi gain
int fCenter(int s, int gain) {
  int offset = signal_min + (signal_span/2);
  return fAdjust(s,gain,offset);
}
