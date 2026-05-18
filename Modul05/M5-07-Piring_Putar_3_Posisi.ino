/*M5-07-Piring_Putar_3_Posisi
 * Program piring putar motor stepper dengan 3 tombol dan 3 posisi
 * - menggunakan library Stepper.h
 * - S0 (BT0/Tombol kiri)   : memanggil jarum ke P0 (0°)
 * - S1 (BT1/Tombol tengah) : memanggil jarum ke P1 (120°)
 * - S2 (BT2/Tombol kanan)  : memanggil jarum ke P2 (240°)
 * - Jarum bergerak via jarak sudut terpendek (CW atau CCW)
 * - Jika tombol ditekan saat motor bergerak, gerakan dibatalkan
 *   dan jarum langsung menuju posisi baru via jarak terpendek
 *
 * Copyright: Revisi oleh Jhon Christabel Fausta Silalahi - 13323054
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

// Objek stepper: urutan pin IN1,IN3,IN2,IN4 agar fasa benar untuk 28BYJ-48
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// ── Posisi Target ─────────────────────────────────────────────
// P0=0°, P1=120°, P2=240° — dikonversi ke satuan steps
const int TARGET_DEG[3]   = { 0, 120, 240 };
const char* TARGET_NAME[3] = { "P0 (0°)", "P1 (120°)", "P2 (240°)" };

// Hitung steps dari derajat (pakai round agar tidak ada error float)
int degToSteps(float deg) {
  return (int)round(deg / step_angle);
}

const int TARGET_STEPS[3] = {
  0,
  degToSteps(120.0f),   // ~682 steps
  degToSteps(240.0f)    // ~1365 steps
};

// ── State Machine ─────────────────────────────────────────────
// DIAM   : motor berhenti, menunggu tombol
// MOVE   : motor sedang bergerak menuju target
// CANCEL : gerakan dibatalkan, target baru menunggu diproses
enum State { DIAM, MOVE, CANCEL };
State state = DIAM;

// bt_bin: bitmask tombol hasil a2d()
// BT0 (kiri)   → bit 0 → nilai 1  → target P0
// BT1 (tengah) → bit 1 → nilai 2  → target P1
// BT2 (kanan)  → bit 2 → nilai 4  → target P2
byte bt_bin = 0;

// ── State Variables ───────────────────────────────────────────
int  currentPosition = 0;       // posisi saat ini dalam steps (0 ~ stepsPerRevolution-1)
int  requestedTarget = -1;      // index target yang diminta (0/1/2), -1 = tidak ada
bool cancelFlag      = false;   // flag pembatalan gerakan

// ── Fungsi Utilitas ───────────────────────────────────────────

/**
 * Hitung jarak terpendek (dalam steps) dari posisi `from` ke `to`.
 * Return: positif = CW, negatif = CCW
 */
int shortestPath(int from, int to) {
  int diff = ((to - from + stepsPerRevolution * 3 / 2) % stepsPerRevolution)
             - stepsPerRevolution / 2;
  return diff;
}

/**
 * Baca tombol dari bt_bin, kembalikan index target (0/1/2) atau -1.
 * BT0=1 → P0(0), BT1=2 → P1(1), BT2=4 → P2(2)
 */
int getTargetFromButton(byte btn) {
  switch (btn) {
    case 1: return 0;   // BT0 → P0
    case 2: return 1;   // BT1 → P1
    case 4: return 2;   // BT2 → P2
    default: return -1; // tidak ada tombol ditekan
  }
}

// ── State Handlers ────────────────────────────────────────────

/**
 * State DIAM: motor berhenti, polling tombol.
 * Jika ada tombol ditekan → simpan target, pindah ke MOVE.
 */
void stateDiam() {
  int tgt = getTargetFromButton(bt_bin);
  if (tgt < 0) return;  // tidak ada tombol ditekan

  if (currentPosition == TARGET_STEPS[tgt]) {
    Serial.printf("[DIAM] Sudah di %s, tidak perlu bergerak.\n", TARGET_NAME[tgt]);
    return;
  }

  requestedTarget = tgt;
  cancelFlag      = false;
  state           = MOVE;
  Serial.printf("[STATE] DIAM → MOVE menuju %s\n", TARGET_NAME[tgt]);
}

/**
 * State MOVE: eksekusi satu langkah penuh menuju target.
 * Dipanggil berulang dari loop() hingga sampai atau dibatalkan.
 * Strategi: hitung shortestPath setiap iterasi (handle cancel otomatis).
 */
void stateMove() {
  // Cek pembatalan dari tombol baru
  int newTgt = getTargetFromButton(bt_bin);
  if (newTgt >= 0 && newTgt != requestedTarget) {
    Serial.printf("[MOVE] DIBATALKAN! Target baru: %s\n", TARGET_NAME[newTgt]);
    requestedTarget = newTgt;
    cancelFlag      = false;
    // Tetap di state MOVE dengan target baru — tidak perlu ganti state
    return;
  }

  int targetPos = TARGET_STEPS[requestedTarget];
  int delta     = shortestPath(currentPosition, targetPos);

  // Cek apakah sudah sampai
  if (delta == 0) {
    Serial.printf("[MOVE] Tiba di %s (posisi = %d steps)\n",
      TARGET_NAME[requestedTarget], currentPosition);
    state = DIAM;
    return;
  }

  // Tentukan arah dan eksekusi 1 step
  bool cw = (delta > 0);
  myStepper.step(cw ? 1 : -1);

  // Update posisi
  if (cw) {
    currentPosition = (currentPosition + 1) % stepsPerRevolution;
  } else {
    currentPosition = (currentPosition - 1 + stepsPerRevolution) % stepsPerRevolution;
  }
}

// ── Setup & Loop ──────────────────────────────────────────────

void setup() {
  Serial.begin(BAUD);
  myStepper.setSpeed(15);   // 15 rpm

  Serial.println("========================================");
  Serial.println("  PIRING PUTAR 3 POSISI - STEPPER");
  Serial.println("  S0(BT0)→P0  S1(BT1)→P1  S2(BT2)→P2");
  Serial.println("========================================");
  Serial.printf("  Steps/rev : %d\n", stepsPerRevolution);
  Serial.printf("  Step angle: %.4f°\n", step_angle);
  Serial.printf("  P0 = %d steps | P1 = %d steps | P2 = %d steps\n",
    TARGET_STEPS[0], TARGET_STEPS[1], TARGET_STEPS[2]);
  Serial.println("========================================\n");
}

void loop() {
  // ── Baca tombol LEBIH DULU sebelum proses state ──
  int  bt_analog = _btn.aRead();
  bt_bin         = _btn.a2d(bt_analog);

  // Proses state machine
  switch (state) {
    case DIAM:   stateDiam(); break;
    case MOVE:   stateMove(); break;
    default: break;
  }

  delay(5);  // Delay kecil (5ms) agar step berjalan halus; bukan 200ms
}
