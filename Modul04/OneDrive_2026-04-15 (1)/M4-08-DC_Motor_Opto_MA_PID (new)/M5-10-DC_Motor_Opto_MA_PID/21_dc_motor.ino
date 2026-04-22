// ------------------------
// DC Motor

/*
 * Menyiapkan pin untuk kontrol motor DC
 */
void dcMotorSetup() {
  pinMode(pin_motorA, OUTPUT);
  pinMode(pin_motorB, OUTPUT);
  ledcAttachPin(pin_motorA, PWM_CHANNEL);
  ledcSetup(PWM_CHANNEL, motor_freq, PWM_RES); 
}

/* Mengatur kecepatan DC motor
 * pwm = 0 : berhenti
 * pwm 1-255 : putar kanan
 * pwm -(1-255) : putar kiri
 */
void dcMotorGo(int pwm) {
  if (pwm >= 0) {
    ledcWrite(PWM_CHANNEL, pwm);
    digitalWrite(pin_motorB, LOW);
    portENTER_CRITICAL_ISR(&opto_mux);
    motor_direction = 1;
    portEXIT_CRITICAL_ISR(&opto_mux);    
  }
  else {
    digitalWrite(PWM_CHANNEL, 255+pwm);
    digitalWrite(pin_motorB, HIGH);    
    portENTER_CRITICAL_ISR(&opto_mux);
    motor_direction = -1;
    portEXIT_CRITICAL_ISR(&opto_mux);    
  }
}

/* Mengatur kecepatan DC motor sesuai MV
 * akan menyesuaikan PWM dengan histerisis
 */
void dcMotorMV(int mv) {
  int pwm;
  if (mv == 0) {
    dcMotorGo(0);
  }  
  if (mv > 0) {
    pwm = min(PWM_MAX, mv+PWM_HISTERISIS);
    dcMotorGo(pwm);
  }
  else {
    pwm = max(-PWM_MAX, mv-PWM_HISTERISIS);
    dcMotorGo(pwm);
  }
}
