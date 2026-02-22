/* Program : Pengolahan data mahasiswa 
* Contoh I/O Serial  
*/ 
// Konstanta, pakai model lama  
#define BOBOT_UTS 0.4  
#define BOBOT_UAS 0.6 
// konstanta, gaya baru 
const float bA = 85.0; 
const float bB = 75.0; 
const float bC = 60.0; 
const float bD = 40.0; 
// Data mahasiswa  
long nim;  
String nama;  
float uts;  
float uas; 
float nilai_angka;  
char nilai_huruf; 
/* menampilkan prompt dan tunggu jawaban */  
void tanya(char s[]) { 
// habiskan data sebelumnya  
  while (Serial.available()) 
    Serial.read(); 
// cetak prompt  
  Serial.print(s);  
  Serial.print(": "); 
// tunggu sampai ada data  
while (!Serial.available()) {} 
} 
/* input data */ 
void inputData() { 
  Serial.println("Masukkan data"); 

  tanya("NIM"); 
  nim = Serial.parseInt(); 
  Serial.println(nim); 

  tanya("Nama"); 
  nama = Serial.readStringUntil('\n'); 
  Serial.println(nama);

  tanya("UTS"); 
  uts = Serial.parseFloat(); 
  Serial.println(uts); 

  tanya("UAS");
  uas = Serial.parseFloat(); 
  Serial.println(uas); 
} 
/* hitung nilai angka dari nilai uts dan uas */  
float nilaiAngka(float uts, float uas) { 
  return (BOBOT_UTS*uts)+(BOBOT_UAS*uas); 
} 
/* hitung nilai huruf berdasar batas bA, bB, bC, bD */  
char nilaiHuruf(float nangka) { 
  if (nangka >= bA) {
    return 'A';
  }
  else if (nangka < bA && nangka >= bB) {
    return 'B';
  }
  else if (nangka < bB && nangka >= bC) {
    return 'C';
  }
  else if (nangka < bC && nangka >= bD) {
    return 'D';
  }
  else {
     return 'E'; 
  }
} 
/* cetak data */ 
void cetakData() { 
  char buffer[60]; // variabel lokal 
  sprintf(buffer, "%8ul %-15s %6.2f %c", 
    nim, nama.c_str(), nilai_angka, nilai_huruf); 
  Serial.println(buffer); 
  Serial.println("----------"); 
} 
// inisialisasi  
void setup() { 
  Serial.begin(115200); 
  Serial.println();  
  Serial.println("Pengolahan Nilai"); 
} 
// loop utama 
void loop() { 
  inputData(); 
  nilai_angka = nilaiAngka(uts, uas);  
  nilai_huruf = nilaiHuruf(nilai_angka);  
  cetakData(); 
} 