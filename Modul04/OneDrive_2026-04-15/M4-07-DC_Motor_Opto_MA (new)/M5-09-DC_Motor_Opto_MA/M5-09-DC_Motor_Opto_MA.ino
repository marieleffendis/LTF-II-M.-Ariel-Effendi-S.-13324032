/* Program : DC Motor Opto MA
 * Program ini mengukur kecepatan motor 
 * dengan opto coupler yang moving average
 * sehingga pengukuran lebih responsif
 * Untuk mencoba:
 * - jalankan Serial Plotter
 * - Masukkan PWM (meloncat, misal dari 0 ke 128
 * - Amati respon kecepatan motor
 * 
 * (c) Eko M. Budi, 2022
 */

#include <TFScope22.h>
#include <TFPeriodic.h>
#include <TFRingBuffer.h>

#define BAUD 500000
#define PROG_NAME "Motor DC Opto MA"
