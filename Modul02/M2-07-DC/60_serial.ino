/* Modul Serial
 * Fungsi untuk menangani masukan/luaran ke serial
 */

// Menampilkan prompt
void serialPrompt(char* s) {
  // habiskan data sebelumnya
  while (Serial.available()) 
    Serial.read();
    
  // cetak prompt
  Serial.print(s);
  Serial.print(": ");

}
