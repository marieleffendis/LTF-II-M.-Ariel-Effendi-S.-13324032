/*M5-02-Kalibrasi_Motor_Servo
 * Program menggerakkan jarum motor servo setiap derajat
 * mulai 0 s.d. 180 derajat dan sebaliknya
 * - tanpa menggunakan library Servo.h
 * - menggunakan pembangkitan sinyal PWM manual
 * - servo SG90 Tower Pro (500 - 2400 us)
 */

#define BAUD      500000

// === inisialisasi variabel ===
int servo  = 13;
int sudut;
int t_pwm; // periode duty cycle

void servoPulse(int servo, int sudut) {
  t_pwm = (int)(500 + (sudut / 180.0) * 1900);
    
  digitalWrite(servo, HIGH);
  delayMicroseconds(t_pwm);
  digitalWrite(servo, LOW);
  delayMicroseconds(20000 - t_pwm); 
}

void printHeader() {
  Serial.println("Sudut  PWM");
}

void printMotor() {
  char str[60];
  sprintf(str, "%03d %04d", sudut, t_pwm);
  Serial.println(str);
}

void setup() {
  Serial.begin(BAUD);
  pinMode(servo, OUTPUT);
  printHeader();
  servoPulse(servo, 0);
}

void loop() {
 
  for (sudut = 0; sudut <= 180; sudut++) {
    servoPulse(servo, sudut);
    printMotor();
  }

  for (sudut = 180; sudut >= 0; sudut--) {
    servoPulse(servo, sudut);
    printMotor();
  }
}
