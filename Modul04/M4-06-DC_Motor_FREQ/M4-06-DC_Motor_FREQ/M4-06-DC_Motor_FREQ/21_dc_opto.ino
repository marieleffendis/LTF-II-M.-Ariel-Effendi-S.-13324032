//--------------------------------------------
// OPTO Coupler

void IRAM_ATTR optoISR() {
  portENTER_CRITICAL_ISR(&opto_mux);
  opto_counter++;
  portEXIT_CRITICAL_ISR(&opto_mux);
}

/* Menghitung RPM
 * 
 */
unsigned optoGetRPM() {
  unsigned long count;
  portENTER_CRITICAL_ISR(&opto_mux);
  count = opto_counter;
  opto_counter=0;
  portEXIT_CRITICAL_ISR(&opto_mux);
  unsigned long now = millis();
  unsigned long interval = now - opto_last_time;
  opto_last_time = now;  

  // PERBAIKI, sesuaikan dengan rumus RPM  
  return count/2*60;
}

/* Menyiapkan opto coupler
 *  
 */
void optoSetup() {
  opto_counter=0;
  pinMode(pin_opto, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_opto), optoISR, FALLING); 
  opto_last_time = millis();
}

/*
 * Opto selesai bekerja
 */
void optoStop() {
  detachInterrupt(digitalPinToInterrupt(pin_opto)); 
}
