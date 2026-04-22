/* Program utama
 * - Menunggu user memasukkan angka freq (1 - 10000)
 * - mengubah PWM agar beroperasi dengan freq tersebut
 * - memberi power PWM=50% ke motor 
 * - mengukur kecepatan motor sebenarnya dengan opto
 */

Periodic t1(ts_display);

int readFreq() {
  int value = Serial.parseInt();
  // habiskan sisa di buffer
  while (Serial.available()) Serial.read();
  return constrain(value, 1, 10000);
}

void printMotor() {
  char str[60];
  sprintf(str, "%04d %05d", motor_freq, motor_rpm);
  Serial.println(str);
}

void printHeader() {
  Serial.println("FREQ  RPM");  
}

void setup() {
  Serial.begin(BAUD);
  Serial.println();  
  Serial.println(PROG_NAME);  
  
  optoSetup();
  dcMotorSetup();  
  motor_pwm = PWM_MAX;  // duty cycle 50%
  Serial.println("Ketikkan Freq (Hz):");  
  printHeader();
}

void loop() {
  if (t1.isTime()) {
    motor_rpm = optoGetRPM();
    printMotor();
  }  
  else if (Serial.available()) {
    motor_freq = readFreq();
    dcMotorSetup();
    dcMotorGo(motor_pwm);
    printHeader();
  }
}
