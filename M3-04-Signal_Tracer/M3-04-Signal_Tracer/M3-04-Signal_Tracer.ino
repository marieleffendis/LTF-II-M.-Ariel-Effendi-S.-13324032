/* Signal Tracer
 * Menguji pengaruh waktu sampling signal tracer
 * terhadap kemulusan sinyal yang bisa dibaca
 */

// pustaka
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TFPeriodic.h>
#include <TFScope22.h>

#define BAUD 9600
#define PROG_NAME "Signal Generator & Tracer"
