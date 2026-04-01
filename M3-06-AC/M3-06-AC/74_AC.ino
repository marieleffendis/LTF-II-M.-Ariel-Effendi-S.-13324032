/*
 * Pengukuran AC (Analisis Spektrum)
 */

#include <arduinoFFT.h> // Pastikan pustaka ini terinstal

// Persiapan data FFT
double vReal[N_DATA];
double vImag[N_DATA];
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, N_DATA, (double)(1000000.0 / ts_signal));

void acPrintHeader() {
  char buff[60];
  sprintf(buff, "Freq(Hz) Magnitudo %s Periode=%d", 
    signal_strs[signal_type], periode_signal);
  Serial.println(buff);
}

void acPrintSpektrum() {
  // Menampilkan hasil FFT ke Serial
  for (int i = 0; i < (N_DATA / 2); i++) {
    double freq = (i * 1.0 * (1000000.0 / ts_signal)) / N_DATA;
    Serial.print(freq, 2);
    Serial.print(" ");
    Serial.println(vReal[i], 2);
  }
}

// fungsi pengukuran AC
void acMeasure() {
  // Re-use logic dari trMeasure untuk mengambil data transien terlebih dahulu
  trMeasure(); 

  int item_mv[N_SIGNAL + 1];
  int count = 0;

  // Pindahkan data dari buffer ke array FFT (mengambil Channel V0/item_mv[1])
  while (!lbuffer.isEmpty() && count < N_DATA) {
    lbuffer.take(item_mv);
    vReal[count] = (double)item_mv[1]; // Mengambil data ADC pertama
    vImag[count] = 0.0;
    count++;
  }

  // Proses FFT
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();
}

// fungsi aksi utama untuk perintah AC
void acAction() {
  Serial.println("--- AC Spectrum Analysis ---");
  acMeasure();
  acPrintHeader();
  acPrintSpektrum();
}
