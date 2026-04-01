/* 
 * Data global untuk operasi OP,DC,TR,AC
 */ 

// banyak pin DA 
#define N_DA   1
#define N_AD   2

#define N_SIGNAL (N_DA+N_AD)
#define N_DATA   500

// Pin I/O
int p_da = AO0;
int p_ad[N_AD] = {AI0, AI1};

// data pengukuran
int ad_mv[N_AD];  
int da_mv;
int ina_mv;
int ina_ma;
int ina_mw;

LinearBuffer lbuffer(N_DATA, N_SIGNAL); // buffer banyak data

// signal generator
unsigned periode_signal = 1000;
unsigned n_signal = 100;
unsigned ts_signal = 100;

// signal tracer
unsigned ts_trace = 1000;
int n_trace=N_DATA;

// Periodic
Periodic p1(0);
Periodic p2(0);
