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
#include <math.h> // Penting untuk fungsi pow() dan sqrt() [cite: 43]

void opAction(int op_mv) {
  opPrintHeader(); 
  da_mv = op_mv;   
    
  // 1. Siapkan variabel akumulator untuk rata-rata dan standar deviasi
  float sum_ina = 0, sum_sq_ina = 0;
  float sum_ad[N_DA] = {0};
  float sum_sq_ad[N_DA] = {0};

  // Output ke DAC
  dacWriteMv(p_da, da_mv);
  delay(100); // Tunggu kondisi tunak

  // 2. Loop Pengambilan Data
  for (int i = 0; i < ndata; i++) { 
    // Baca data sensor
    ina_mv = inaReadLoadMv(); 
    
    // Akumulasi INA
    sum_ina += (float)ina_mv;
    sum_sq_ina += pow((float)ina_mv, 2);

    for(int j = 0; j < N_DA; j++) {
      ad_mv[j] = analogReadMv(p_ad[j]); 
      
      // Akumulasi ADC
      sum_ad[j] += (float)ad_mv[j];
      sum_sq_ad[j] += pow((float)ad_mv[j], 2);
    }

    opPlot(); 
  }  

  // 3. Perhitungan Statistik
  Serial.println("--------------------------------------------");
  
  // Hitung Mean
  float mean_ina = sum_ina / ndata;
  
  // Hitung Standar Deviasi: sqrt( (Σx² / N) - (mean²) )
  float std_ina = sqrt((sum_sq_ina / ndata) - pow(mean_ina, 2));

  // Tampilkan Rata-rata
  Serial.print("MEAN:  ");
  char mean_buf[50];
  sprintf(mean_buf, "      %4.1f %4.1f %4.1f %4.1f", 
          mean_ina, sum_ad[0]/ndata, sum_ad[1]/ndata, sum_ad[2]/ndata);
  Serial.println(mean_buf);

  // Tampilkan Standar Deviasi
  Serial.print("STDEV: ");
  float std_ad[N_DA];
  for(int j = 0; j < N_DA; j++) {
    std_ad[j] = sqrt((sum_sq_ad[j] / ndata) - pow(sum_ad[j] / ndata, 2));
  }
  
  char std_buf[50];
  sprintf(std_buf, "      %4.2f %4.2f %4.2f %4.2f", 
          std_ina, std_ad[0], std_ad[1], std_ad[2]);
  Serial.println(std_buf);
  Serial.println("--------------------------------------------");
}
