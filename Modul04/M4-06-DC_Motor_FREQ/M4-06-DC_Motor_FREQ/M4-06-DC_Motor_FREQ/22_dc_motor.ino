/*
 * Menyiapkan pin untuk kontrol motor DC
 */
void dcMotorSetup() {
  pinMode(pin_motorA, OUTPUT);
  pinMode(pin_motorB, OUTPUT);
  ledcAttach(pin_motorA, motor_freq, PWM_RES);
}

/* Mengatur kecepatan DC motor
 * pwm = 0 : berhenti
 * pwm 1-255 : putar kanan
 * pwm -(1-255) : putar kiri
 */
void dcMotorGo(int pwm) {
  if (pwm >= 0) {
    ledcWrite(pin_motorA, pwm);
    digitalWrite(pin_motorB, LOW);
  }
  else {
    ledcWrite(pin_motorA, 255+pwm);
    digitalWrite(pin_motorB, HIGH);
  }  
}
