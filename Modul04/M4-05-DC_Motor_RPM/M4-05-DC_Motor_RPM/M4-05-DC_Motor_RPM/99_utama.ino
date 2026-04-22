/* Program utama
 * - Menunggu user memasukkan angka pwm (-255 - 255)
 * - memberi power ke motor sesuai angka tersebut
 * - mengukur kecepatan motor sebenarnya dengan opto
 */

Periodic t1(ts_display);

int readPWM() {
  int pwm = Serial.parseInt();
  // habiskan sisa di buffer
  while (Serial.available()) Serial.read();
  return constrain(pwm,-PWM_MAX, PWM_MAX);
}

void printMotor() {
  char str[60];
  float current_ma = _ina219.getCurrent_mA();
  sprintf(str, "%04d %05d %6.2f", motor_pwm, motor_rpm, current_ma);
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
  Serial.println("Ketikkan PWM:");  
  printHeader();
  
  if (!_ina219.begin()) {
    while (1) { delay(100); }
  }
}

void loop() {
  if (t1.isTime()) {
    motor_rpm = optoGetRPM();
    printMotor();
  }  
  else if (Serial.available()) {
    motor_pwm = readPWM();
    dcMotorGo(motor_pwm);
    printHeader();
  }
}
