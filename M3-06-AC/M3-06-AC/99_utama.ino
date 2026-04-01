/*
 * Program utama 
 * - menerima masukan dari serial dengan format
 *   [perintah] [periode_ms]
 * - lalu menjalankan pengukuran transien atau AC
 *   
 * Coba jalankan dan amati luaran dengan Serial Plotter,
 * beri masukan, misalnya:
 *   TS 1000
 *   AC 
 */ 

// Rentang periode
#define PERIODE_MIN 10
#define PERIODE_MAX 10000


void parseAC() {
  // tak ada parameter lain, langsung 
  acAction();  
}

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
    else if (cmd == "AC") {
      Serial.println("HI");
      parseAC();
    }
    // habiskan semua sisa di serial
    while(Serial.available()) Serial.read();
}

// Utama, algoritma multi-loop periodik
void setup() {
  Serial.begin(500000);    
  delay(3000);
  Serial.println();
  Serial.println(PROG_NAME);
}

void loop() {
  if (Serial.available()) {
    parseCommand();
  }
}
