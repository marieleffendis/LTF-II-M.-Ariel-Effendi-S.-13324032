/* Program : OP
 * Pengukuran Operating Point
 * 
 * Dibagi menjadi multifile
 * - sketch pertama : library & konstanta
 * - 10_escope : fungsi dasar escope
 * - 60_serial : fungsi dasar serial
 * - 70_data : deklarasi data global
 * - 71_op : fungsi untuk operasi OP
 * - 99_main : fungsi utama setup() dan loop()
 * 
 * (c) Eko M. Budi, 2022
 */

// pustaka
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TFScope22.h>

#define BAUD 500000
#define PROG_NAME "Pengukuran Operating Point"
