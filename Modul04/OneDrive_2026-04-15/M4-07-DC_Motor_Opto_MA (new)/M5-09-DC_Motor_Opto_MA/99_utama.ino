/* Program utama
 * - Menunggu user memasukkan angka 
 * - mengubah PWM sesuai angka
 * - mengukur kecepatan motor 
 * - menampilkan hasil pengukuran
 * 
 * Bisa coba ubah kinerja pengukuran
 * dengan mengganti ts_measure dan ts_display
 */

Periodic t1(ts_measure);
Periodic t2(ts_display);

int readPWM() {
  int value = Serial.parseInt();
  // habiskan sisa di buffer
  while (Serial.available()) Serial.read();
  return constrain(value, -PWM_MAX, PWM_MAX);
}

void printMotor() {
  char str[60];
  sprintf(str, "%04d %05d", motor_pwm, motor_rpm);
  Serial.println(str);
}

void printHeader() {
  Serial.println("PWM  RPM");  
}

void setup() {
  Serial.begin(BAUD);
  Serial.println();  
  Serial.println(PROG_NAME);  
  
  optoSetup();
  dcMotorSetup();  
  printHeader();
}

void loop() {
  if (t1.isTime()) {
    optoMeasure();
  }  
  else if (t2.isTime()) {
    motor_rpm = optoGetRPM();
    printMotor();
  }
  else if (Serial.available()) {
    motor_pwm = readPWM();
    dcMotorGo(motor_pwm);
    printHeader();
  }
}
