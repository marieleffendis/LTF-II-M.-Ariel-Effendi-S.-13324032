/* 
 * Data global untuk operasi 
 */ 

// banyak pin DA 
#define N_DA 3

// Pin I/O
int p_ad[N_DA] = {DI0, DI1, DI2};
int p_da = AO0;

// banyak data pengukuran
int ndata=20;

// data pengukuran
int ad_mv[N_DA];  
int da_mv;
int ina_mv;
