// ------------------------
// Kontrol PID untuk motor

#define PID_SUM_MAX 10

float pid_error;
float pid_sum;

/*
 * Menyiapkan PID untuk mulai bekerja
 */
void pidSetup() {
  pid_sum = 0;
  pid_error = 0;
}

/* Menghitung kontrol sesuai rumus PID
 */
float pidControl(float sv, float pv) {
  curr_time = millis();
  float delta_time = curr_time - prev_time;

  float e = sv - pv;
  float de = (e - pid_error) / delta_time;
  pid_error = e;
  pid_sum += e * delta_time;
  
  prev_time = millis();

  // batasi harga pid_sum 
  if (pid_sum > PID_SUM_MAX) 
    pid_sum=PID_SUM_MAX;
  else if (pid_sum < -PID_SUM_MAX) 
    pid_sum=-PID_SUM_MAX;
  
  return (pid_kp * e) + (pid_ki*pid_sum) + (pid_kd * de);
}
