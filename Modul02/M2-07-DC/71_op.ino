/*
 * Action Operating Condition
 */

void opPrintHeader() {
  Serial.println("VDA  Vi+  V0   V1   V2");
}

void opPlot() {
  char buff[40];
  sprintf(buff, "%4d %4d %4d %4d %4d", 
    da_mv, ina_mv, ad_mv[0], ad_mv[1], ad_mv[2]);
  Serial.println(buff);  
}

// Melaksanakan prosedur OP
// Kirim output tertentu ke DA
// lalu ukur dan print ke Serial sebanyak ndata
// lalu hitung rata-rata
void dcSweepAction(int start_mv, int stop_mv, int step_mv) {
  opPrintHeader(); 
  
  // Perulangan utama: menaikkan tegangan berdasarkan step
  for (int v = start_mv; v <= stop_mv; v += step_mv) {
    da_mv = v; // Update nilai global da_mv [cite: 39]
    
    // Kirim ke DAC
    dacWriteMv(p_da, da_mv); 
    delay(200); // Beri waktu lebih lama agar tegangan benar-benar naik

    // Baca hasil pengukuran
    ina_mv = inaReadLoadMv(); // [cite: 21]
    for(int i=0; i<N_DA; i++) {
      ad_mv[i] = analogReadMv(p_ad[i]); 
    }

    // Tampilkan baris data saat ini
    opPlot(); 
  }
  
  // Reset ke 0 setelah selesai
  dacWriteMv(p_da, 0); 
}
