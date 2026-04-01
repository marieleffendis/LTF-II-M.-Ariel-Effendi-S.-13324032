/*
 * Program utama 
 * - menerima masukan dari serial dengan format
 *   [jenis sinyal] [periode_ms]
 * - lalu menjalankan pengukuran transien
 *   
 * Coba jalankan dan amati luaran dengan Serial Plotter,
 * beri masukan, misalnya:
 *   TS 1000
 *   TR 500
 *   TT 100
 *   TW 50
 */ 

// Rentang periode
#define PERIODE_MIN 10
#define PERIODE_MAX 10000

// parsing perintah transient lanjutan
void parseTX(int st) {
  // baca parameter periode
  unsigned prd = Serial.parseInt();
  if ((prd >= PERIODE_MIN) && (prd <= PERIODE_MAX)) {
      // kalau memenuhi lakukan aksi
      periode_signal = prd;  // set periode
      signal_type = st;  // set bentuk gelombang      
      trAction();
  }  
}

// parsing perintah awal
void parseCommand() {
    // baca perintah 
    String cmd = Serial.readStringUntil(' ');
    cmd.trim();
    cmd.toUpperCase();
    
    if (cmd == "TS") {
      parseTX(ST_SINUS);
    }
    else if (cmd == "TR") {
      parseTX(ST_RECTANGLE);
    }
    else if (cmd == "TT") {
      parseTX(ST_TRIANGLE);
    }
    else if (cmd == "TW") {
      parseTX(ST_SAWTOOTH);
    }
    // habiskan semua sisa di serial
    while(Serial.available()) Serial.read();
}

// Utama, algoritma multi-loop periodik
void setup() {
  Serial.begin(BAUD);    
  delay(3000);
  Serial.println();
  Serial.println(PROG_NAME); 
}

void loop() {
  if (Serial.available()) {
    parseCommand();
  }
}
