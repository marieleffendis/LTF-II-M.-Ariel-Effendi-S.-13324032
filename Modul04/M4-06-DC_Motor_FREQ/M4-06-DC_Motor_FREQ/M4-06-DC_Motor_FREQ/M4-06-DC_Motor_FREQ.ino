/* Program : DC Motor FREQ
 * Program ini mengamati efek frekuensi PWM pada motor
 * - Jalankan Serial Monitor
 * - Ketik frequensi PWM (1 - 10000)
 * - Amati FREQ dan RPM Motor
 * 
 * (c) Eko M. Budi, 2022
 */

#include <TFScope22.h>
#include <TFPeriodic.h>

#define BAUD 500000
#define PROG_NAME "Motor DC FREQ Test"
