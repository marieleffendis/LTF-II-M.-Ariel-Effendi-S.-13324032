/*
 * Data untuk DC motor dan Opto Coupler
 */

// konstanta untuk PWM motor
#define PWM_CHANNEL 0
#define PWM_RES   8

#define PWM_MIN 0
#define PWM_MAX 255

// RPM sesuai kemampuan motor
#define RPM_MAX 200

// histerisis sesuai motor
#define PWM_HISTERISIS  0

// pin yang digunakan
int pin_motorA = DO0;
int pin_motorB = DO1;
int pin_opto = DI0;

// variabel kontrol/pengukuran motor
int motor_freq = 15;  // sesuaikan yang tepat untuk motor
int motor_pwm = 0;    // manipulated variable (MV) bagi PID
int motor_rpm = 0;    // process variabel (PV) bagi PID
int motor_sp = 0;     // set-point (SV) bagi PID
int motor_direction = 1; // arah putaran

// Parameter PID
float pid_kp = 1.0;
float pid_ki = 0;
float pid_kd = 0;

// timing
unsigned long curr_time = millis();
unsigned long prev_time = curr_time;

// variabel pengukuran opto_coupler
portMUX_TYPE opto_mux = portMUX_INITIALIZER_UNLOCKED;
volatile unsigned opto_counter = 0;
unsigned long opto_last_time = 0;

// Menggunakan Ring Buffer untuk moving average
// pengukuran kecepatan motor

#define N_WINDOW 100
struct OptoData {
  uint16_t interval;
  uint16_t count;
};

RingBuffer<N_WINDOW, OptoData> obuff;
unsigned long opto_sum;
unsigned long opto_interval;

// periode task 
unsigned ts_display = 10; // display setiap 1000 ms
unsigned ts_measure = 10;   // kontrol setiap 10 ms
