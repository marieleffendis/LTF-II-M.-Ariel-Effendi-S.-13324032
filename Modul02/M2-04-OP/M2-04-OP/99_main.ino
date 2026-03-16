// UTAMA
void setup() {
  // Siapkan serial, tunggu sampai terkoneksi
  Serial.begin(BAUD);
  while (!Serial);
  Serial.println();
  Serial.println(PROG_NAME);

  // siapkan board ESCOPE
  escopeSetup();  
}

// loop tunggu user input, lalu proses
void loop() {
  // minta masukan dari user
  serialPrompt("OP");

  // tunggu sampai memang ada kiriman
  while(!Serial.available()) {
    oledPrint(0,0,"OP");
    delay(500);    
    oled.clearDisplay();
    delay(500);    
  }
  
  int mv0 = Serial.parseInt();
  mv0 = constrain(mv0,100,3100);
  Serial.println(mv0);

  // lakukan aksi OP
  opAction(mv0);  
}
