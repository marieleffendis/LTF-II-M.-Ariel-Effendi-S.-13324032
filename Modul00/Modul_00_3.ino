/* Program : Pengolahan data mahasiswa
 * Contoh lanjut penggunaan struktur data (struct, array)
 * Dan algoritma (looping, pencabangan)
 * (c) Eko M. Budi, 2022
 */

// Konstanta, pakai model lama
#define BOBOT_UTS 0.4
#define BOBOT_UAS 0.6

// untuk data mahasiswa, gunakan struct
struct Mahasiswa {
  long nim;
  String nama;
  float uts;
  float uas;
  float nilai_angka;
  char nilai_huruf;
};

// data mahasiswa awal, langsung inisialisasi
#define N_SISWA 5
Mahasiswa siswa[N_SISWA] = {
  {13300991, "Eko"},
  {13300992, "Dwiki"},
  {13300993, "Trias"},
  {13300994, "Catur"},
  {13300995, "Panca"}
};

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

/* input data N mahasiswa */
void inputData() {
  // pakai looping
  for (int i=0; i<N_SISWA; i++) {
    Serial.print(siswa[i].nim);
    Serial.print(" ");
    Serial.println(siswa[i].nama);

    tanya("UTS");
    siswa[i].uts = Serial.parseFloat();
    Serial.println(siswa[i].uts);
  
    tanya("UAS");
    siswa[i].uas = Serial.parseFloat();
    Serial.println(siswa[i].uas);  
  }
}

/* hitung nilai angka N mahasiswa */
void nilaiAngka() {
  for (int i=0; i<N_SISWA; i++) {
    siswa[i].nilai_angka = 
      (BOBOT_UTS*siswa[i].uts)+
      (BOBOT_UAS*siswa[i].uas);
  }
}

/* hitung rata-rata dari semua mahasiswa */
float rerata() {
  float sum=0; 
  for (int i=0; i<N_SISWA; i++) {
    sum = sum + siswa[i].nilai_angka;
  }
  return sum / N_SISWA;
}

/* hitung standar deviasi dari semua mahasiswa
 * input nr = nilai rata-rata
 * return standar deviasi
 */
float stdDev(float nr) {
  // hitung standar deviasi di sini
  float std;
  for (int i = 0; i < N_SISWA ; i++){
    std = pow(nr - siswa[i].nilai_angka , 2);
  }
  return pow(std / N_SISWA, 0.5);
}


/* hitung nilai huruf semua mahasiswa 
 * Input: nr = rerata
 *       sd = standard deviasi
 * Proses: semua mahasiswa akan terupdate nilai_hurufnya
 */
void nilaiHuruf(float nr, float sd) {
  // tentukan dulu batas nilai huruf
  float batas_a = nr+sd;
  float batas_b = nr;
  float batas_c = nr-sd;
  float batas_d = nr-2*sd;

  for (int i=0; i<N_SISWA; i++) {
    // gunakan pencabangan untuk menentukan nilai huruf
    if (siswa[i].nilai_angka >= batas_a) {
      siswa[i].nilai_huruf = 'A';
    }
    else if (siswa[i].nilai_angka < batas_a && siswa[i].nilai_angka >= batas_b) {
      siswa[i].nilai_huruf = 'B';
    }
    else if (siswa[i].nilai_angka < batas_b && siswa[i].nilai_angka >= batas_c) {
      siswa[i].nilai_huruf = 'C';
    }
    else if (siswa[i].nilai_angka < batas_c && siswa[i].nilai_angka >= batas_d) {
      siswa[i].nilai_huruf =  'D';
    }
    else {
      siswa[i].nilai_huruf =  'E'; 
    } 
  }
}

String deskripsi(char nilai_huruf) {
  String d;
  switch (nilai_huruf){
    case 'A':
      return "Amat Bagus";
    case 'B':
      return "Bagus";
    case 'C':
      return "Cukup";
    case 'D':
      return "Kurang";
    case 'E':
      return "Tidak lulus";
  }
}

void cetakData(float nr, float sd) {
  char sbuffer[60];  // variabel lokal
  String d;
  char fheader[]= "%8s %-15s %6s %5s %s";
  char fbaris[] = "%8lu %-15s %6.2f %5c %s";
  char garis[] = "---------------------------------------";  

  // cetak rata-rata & stdev
  Serial.print("Rerata Kelas: ");  
  Serial.println(nr);  
  Serial.print("StDev Kelas : ");  
  Serial.println(sd);  
  // cetak header
  Serial.println(garis);
  sprintf(sbuffer, fheader,
      "NIM", "Nama", "Angka", "Huruf", "Deskripsi");
  Serial.println(sbuffer);  
  Serial.println(garis);

  // cetak semua baris
  for (int i=0; i<N_SISWA; i++) {
    d = deskripsi(siswa[i].nilai_huruf);
    sprintf(sbuffer, fbaris,
      siswa[i].nim, 
      siswa[i].nama.c_str(), 
      siswa[i].nilai_angka, 
      siswa[i].nilai_huruf, 
      d.c_str());
    Serial.println(sbuffer);
  }
  Serial.println(garis);  
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
  nilaiAngka();
  float nr = rerata();
  float sd = stdDev(nr);
  nilaiHuruf(nr, sd);  
  cetakData(nr, sd);    
}
