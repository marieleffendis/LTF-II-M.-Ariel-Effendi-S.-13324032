/* Program Flash Buffer
 * Contoh memakai flash buffer
 *
 * Jalankan dan lihat luaran di Serial Monitor
 * Ketikkan data (angka) untuk melihat isian buffer
 * Matikan / reset ESP32 untuk cek apakah data tersimpan.
 * 
 * (c) Eko M. Budi, 2022
 */

#include <TFFlashBuffer.h>

//----------------------------------------------------------
#define BAUD 500000

// baca 1 line dari Serial sampai RETURN, atau n_line terbaca
// return banyak huruf terbaca
int readLine(char prompt[], char line[], int n_line) {
  // habiskan input
  while(Serial.available()) Serial.read();
  Serial.print(prompt);
  Serial.print("= ");
  int i;
  for (i=0; i<n_line-1; i++) {
    while (!Serial.available());
    char ch = Serial.read();
    if ((ch == '\n') || (ch == '\r')) break;
    line[i] = ch;
  }
  line[i] = '\0';
  return i;  
}

//-----------------------------------
#define N_ROW 5
#define DTYPE int16_t
#define ADDR_BUFF 0

// buat buffer di EEPROM
FlashBuffer<N_ROW, DTYPE> fbuff(ADDR_BUFF);

// Beberapa buffer tipe data lain, untuk perbandingan size
FlashBuffer<N_ROW,byte> fbuff_byte;
FlashBuffer<N_ROW,int> fbuff_int;
FlashBuffer<N_ROW,long> fbuff_long;
FlashBuffer<N_ROW,float> fbuff_float;

// ambil item terakhir dari Buffer
bool takeBuffer() {
  char str[40];  
  DTYPE data;

  if (!fbuff.take(data)) return false;
  sprintf(str, "Take: %d", data);
  Serial.println(str);
  return true;
}

// cetak Buffer
void printBuffer() {
  char str[40];  
  DTYPE data;
  
  sprintf(str, "FBuffer[%d]: ", fbuff.count());
  Serial.print(str);    
  
  for (int i=0; i<fbuff.count(); i++) {
    fbuff.getFirst(i, data);
    sprintf(str, "%3d ", data);
    Serial.print(str);    
  }
  Serial.println();
}

/* input dan rekam buffer
 */
bool inputBuffer() {
  char str[10];
  if (!readLine("Data", str, 10)) {
    Serial.println();
    return false;
  }

  // dapat data string, konversi ke DTYPE
  DTYPE data;
  data = atoi(str);
  Serial.println(data);

  // tambah ke buffer (otomatis di rekam ke EEPROM)
  fbuff.put(data);
  return true;
}

//---------------------------------
// utilitas melihat isi seluruh EEPROM
void dumpEEPROM(unsigned addr, unsigned sz) {
  char str[10];
  for (int i=0; i<sz; ) {
    byte d = EEPROM.read(addr+i);
    sprintf(str, "%02x ", d);
    Serial.print(str);
    i++;
    if (i%8 == 0) Serial.println();
  }
  Serial.println();
}


void printSize(char *type, unsigned sitem, unsigned sbuff) {
  char str[60];
  sprintf(str, "%-8s %5d %5d", type, sitem, sbuff);
  Serial.println(str);  
}

//----------------------------------------------------
// UTAMA
void setup() {
  Serial.begin(BAUD);
  Serial.println("\nFlashBuffer Test");

  // print berbagai ukuran data
  Serial.print("Buffer dimension = ");
  Serial.println(N_ROW);
  
  Serial.println("type sizeof(type)  buff.memSize");
  Serial.println("-------------------------------");
  printSize("int16_t", sizeof(int16_t), fbuff.memSize());
  printSize("byte", sizeof(byte), fbuff_byte.memSize());
  printSize("int", sizeof(int), fbuff_int.memSize());
  printSize("long", sizeof(long), fbuff_long.memSize());
  printSize("float", sizeof(float), fbuff_float.memSize());

  // Memulai EEPROM
  unsigned mem_size = fbuff.memSize();
  EEPROM.begin(mem_size);
  if (fbuff.load()) {
    Serial.println("EEPROM sudah berisi data");
  }
  else {
    Serial.println("Tak ada rekaman data");
    fbuff.init(); // inisialisasi data kosong
  }
  Serial.println("Masukkan data (integer)");
  Serial.println("Langsung [Enter] akan hapus data pertama");  
  Serial.println("Jika kosong, semua EEPROM akan dihapus");  
}

void loop() { 
  if (!inputBuffer()) {
    if (!takeBuffer()) {
      fbuff.erase();
      Serial.println("EEPROM dihapus, silahkan reboot!");
      for(;;); // berhenti proses    
    }
  }
  printBuffer();

  // unkomen kalau penasaran mau lihat isinya EEPROM
  // dumpEEPROM(ADDR_BUFF, fbuff.memSize());
}
