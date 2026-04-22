/* Program utama
 * - Jika ada, terima set point (SV) dari user
 *   dalam satuan RPM
 * - Secara periodik (cepat)
 *   - mengukur kecepatan motor 
 *   - mengontrol motor dengan PID
 * - Secara periodik (lambat)
 *   - menampilkan pengontrolan
 *   
 * Bisa coba ubah kinerja pengukuran
 * dengan mengganti ts_measure dan ts_display
 * juga pid_kp, pid_ki, pid_kd
 * 
 */

Periodic t1(ts_measure);
Periodic t2(ts_display);

int readRPM() {
  int value = Serial.parseInt();
  // habiskan sisa di buffer
  while (Serial.available()) Serial.read();
  return constrain(value, -2000, 2000);
}

void printMotor() {
  Serial.printf("%04d %04d %04d\n", 
    motor_sp, motor_rpm, motor_pwm);
}

void printHeader() {
  Serial.println("  SV   PV   MV");  
}

void setup() {
  Serial.begin(BAUD);
  Serial.println();  
  Serial.println(PROG_NAME);  
  
  optoSetup();
  dcMotorSetup();  
  pidSetup();
  
  printHeader();
}

void loop() {
  if (t1.isTime()) {
    optoMeasure();
    motor_rpm = optoGetRPM();
    motor_pwm = pidControl(motor_sp, motor_rpm);
    dcMotorGo(motor_sp);
  }  
  else if (t2.isTime()) {
    printMotor();
  }
  else if (Serial.available()) {
    motor_sp = readRPM();
    printHeader();
  }
}
