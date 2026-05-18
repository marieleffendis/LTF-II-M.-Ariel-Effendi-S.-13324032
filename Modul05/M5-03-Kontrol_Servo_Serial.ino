/*M5-03-Kontrol_Servo_Serial
 * Program menggerakkan jarum motor servo melalui Serial Monitor
 * - tanpa menggunakan library Servo.h
 * - menggunakan hasil formula pada M5-02
 */

#define BAUD      500000
#define SUDUT_MIN 0
#define SUDUT_MAX 180
#define PROG_NAME "Kendali Motor Servo"

int servo = 13;
int sudut;
int t_pwm;

// konversi sudut menjadi t_pwm
void servoPulse(int servo, int sudut) {
  t_pwm = (int)(500 + (sudut / 180.0) * 1900); // Konversi sudut ke t_pwm

  for (int i = 0; i < 8; i++) {
    digitalWrite(servo, HIGH);
    delayMicroseconds(t_pwm);
    digitalWrite(servo, LOW);
    delayMicroseconds(20000 - t_pwm);
  }
}

void printHeader() {
  Serial.print("Sudut: ");
}

void printMotor() {
  char str[60];
  sprintf(str, "%03d", sudut);
  Serial.println(str);
}

int readSudut() {
  int sudut = Serial.parseInt();
  while (Serial.available()) Serial.read();
  return constrain(sudut, SUDUT_MIN, SUDUT_MAX);
}

void setup() {
  Serial.begin(BAUD);
  Serial.println();
  Serial.println(PROG_NAME);

  pinMode(servo, OUTPUT);
  Serial.println("Masukkan sudut:");
}

void loop() {
  if (Serial.available()) {
    sudut = readSudut();               // membaca input sudut dari Serial Monitor
    servoPulse(servo, sudut);          // menggerakkan jarum motor sesuai sudut
    printHeader();
    printMotor();
    Serial.println("Masukkan sudut:"); // prompt input berikutnya
  }
}
