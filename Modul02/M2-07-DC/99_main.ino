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
  // 1. Minta input Start
  serialPrompt("DC Sweep - Start (mV)");
  while(!Serial.available()) {
    oledPrint(0,0,"SWEEP"); 
    delay(200);
  }
  int vStart = Serial.parseInt();
  Serial.println(vStart);

  serialPrompt("DC Sweep - Stop (mV)");
  while(Serial.read() != -1); // 
  while(!Serial.available());
  int vStop = Serial.parseInt();
  Serial.println(vStop);

  serialPrompt("DC Sweep - Step (mV)");
  while(Serial.read() != -1); 
  while(!Serial.available());
  int vStep = Serial.parseInt();
  if (vStep <= 0) vStep = 100; 
  Serial.println(vStep);

  Serial.println("\nMemulai DC Sweep...");
  dcSweepAction(vStart, vStop, vStep);
  Serial.println("Sweep Selesai.\n");
  
  delay(2000); 
}
