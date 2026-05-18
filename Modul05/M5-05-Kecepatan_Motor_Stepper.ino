/*M5-05-Kecepatan_Motor_Stepper
 * Program pengukuran kecepatan rotasi motor per menit
 * - menggunakan library Stepper.h
 * - BT0 (Tombol kiri)  : menggerakkan jarum 360° berlawanan arah jam (CCW)
 * - BT2 (Tombol kanan) : menggerakkan jarum 360° searah arah jam (CW)
 * - BT1 (Tombol tengah): memberhentikan gerakan motor
 *   WAJIB tekan BT1 terlebih dahulu sebelum mengubah arah gerakan
 * - Kecepatan rotasi dihitung dari waktu tempuh 1 putaran (360°)
 *   menggunakan millis()
 *
 * Copyright: Revisi oleh Jhon Christabel Fausta Silalahi - 13323054
 * Perbaikan bug: baca tombol sebelum proses state, presisi rpm, 
 *                aturan ganti arah wajib lewat DIAM
 */

#include <Stepper.h>
#include <TFScope22.h>

#define BAUD 500000

// Jumlah langkah per satu putaran penuh (28BYJ-48 via ULN2003 = 2048 steps)
const int   stepsPerRevolution = 2048;
const float step_angle         = 360.0f / stepsPerRevolution; // ~0.1758°/step

// Pin drive motor ULN2003
#define IN1 DO0
#define IN2 DO1
#define IN3 DO2
#define IN4 DO3

// Threshold analog button — sesuaikan dengan nilai AD board masing-masing
int ab_thresholds[] = { 2100, 2400, 2600 };

// Objek kelas analog button
AnalogButton _btn(ab_thresholds);

// State machine
enum State { DIAM, CW, CCW };
State state = DIAM;

// bt_bin: bitmask tombol yang sedang ditekan
// BT0 (kiri)   → bit 0 → nilai 1
// BT1 (tengah) → bit 1 → nilai 2
// BT2 (kanan)  → bit 2 → nilai 4
byte bt_bin = 0;

// Objek stepper: urutan pin IN1,IN3,IN2,IN4 agar fasa benar untuk 28BYJ-48
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// ── Fungsi Helper ─────────────────────────────────────────────

/**
 * Hitung jumlah steps untuk sudut tertentu.
 * Gunakan round() agar tidak ada akumulasi error float.
 */
int degreesToSteps(float degrees) {
  return (int)round(degrees / step_angle);
}

/**
 * Putar motor satu putaran penuh CW, lalu cetak waktu & kecepatan.
 * Arah CW  → step negatif pada library Stepper.h (tergantung wiring).
 */
void rotateCW() {
  Serial.println("Motor bergerak arah CW");

  int    steps        = degreesToSteps(360.0f);
  unsigned long t0    = millis();
  myStepper.step(-steps);                        // CW = negatif
  unsigned long elapsed = millis() - t0;

  float rpm = (elapsed > 0) ? (60000.0f / elapsed) : 0.0f;
  Serial.printf("Waktu tempuh CW  : %lu ms\n", elapsed);
  Serial.printf("Kecepatan    CW  : %.2f rpm\n\n", rpm);
}

/**
 * Putar motor satu putaran penuh CCW, lalu cetak waktu & kecepatan.
 */
void rotateCCW() {
  Serial.println("Motor bergerak arah CCW");

  int    steps        = degreesToSteps(360.0f);
  unsigned long t0    = millis();
  myStepper.step(steps);                         // CCW = positif
  unsigned long elapsed = millis() - t0;

  float rpm = (elapsed > 0) ? (60000.0f / elapsed) : 0.0f;
  Serial.printf("Waktu tempuh CCW : %lu ms\n", elapsed);
  Serial.printf("Kecepatan    CCW : %.2f rpm\n\n", rpm);
}

// ── State Handlers ────────────────────────────────────────────

/**
 * State DIAM: motor berhenti, menunggu BT0 (CCW) atau BT2 (CW).
 * Ganti arah HANYA boleh dilakukan dari state DIAM.
 */
void stateDiam() {
  switch (bt_bin) {
    case 1: state = CCW; Serial.println("[STATE] DIAM → CCW"); break;
    case 4: state = CW;  Serial.println("[STATE] DIAM → CW");  break;
    default: break;
  }
}

/**
 * State CW: motor berputar searah jam.
 * BT1 (case 2) → kembali ke DIAM.
 * BT0 (case 1) → DIABAIKAN, harus lewat DIAM dulu (aturan soal).
 */
void stateCW() {
  switch (bt_bin) {
    case 2:
      state = DIAM;
      Serial.println("[STATE] CW → DIAM (motor berhenti)");
      return;   // jangan lanjut putar
    default:
      break;
  }
  rotateCW();
}

/**
 * State CCW: motor berputar berlawanan arah jam.
 * BT1 (case 2) → kembali ke DIAM.
 * BT2 (case 4) → DIABAIKAN, harus lewat DIAM dulu (aturan soal).
 */
void stateCCW() {
  switch (bt_bin) {
    case 2:
      state = DIAM;
      Serial.println("[STATE] CCW → DIAM (motor berhenti)");
      return;   // jangan lanjut putar
    default:
      break;
  }
  rotateCCW();
}

// ── Setup & Loop ──────────────────────────────────────────────

void setup() {
  Serial.begin(BAUD);
  myStepper.setSpeed(15);   // 15 rpm — bisa disesuaikan

  Serial.println("=== M5-05 Kecepatan Motor Stepper ===");
  Serial.printf("  Steps/rev : %d\n", stepsPerRevolution);
  Serial.printf("  Step angle: %.4f°\n\n", step_angle);
  Serial.println("BT0 (kiri)   = CCW");
  Serial.println("BT1 (tengah) = STOP");
  Serial.println("BT2 (kanan)  = CW");
  Serial.println("=====================================\n");
}

void loop() {
  // ── PERBAIKAN UTAMA: baca tombol LEBIH DULU sebelum proses state ──
  int  bt_analog = _btn.aRead();
  bt_bin         = _btn.a2d(bt_analog);

  // Proses state machine
  switch (state) {
    case DIAM: stateDiam(); break;
    case CW:   stateCW();   break;
    case CCW:  stateCCW();  break;
  }

  delay(200);
}
