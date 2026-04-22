/* Program : DC Motor
 * Program ini mengamati pengaruh PWM terhadap kecepatan motor
 * - Jalankan Serial Monitor
 * - Ketikkan PWM (-255 - 255)
 * - Amati program menampilkan PWM dan RPM 
 * 
 * (c) Eko M. Budi, 2022
 */

#include <TFScope22.h>
#include <TFPeriodic.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

#define BAUD 500000
#define PROG_NAME "Motor DC PWM Test"

Adafruit_INA219 _ina219;
