#ifndef _ALGO_TOP_H_
#define _ALGO_TOP_H_

#include "ap_int.h"

#define N_INPUT_LINKS  32
#define N_OUTPUT_LINKS  1

// TM18_01: B(00, 01, 02, 03, 04, 05), HE(06, 07), HFWD(08, 09), HF(30, 31)
// TM18_07: B(10, 11, 12, 13, 14, 15), HE(16, 17), HFWD(18, 09), HF(30, 31)
// TM18_13: B(20, 21, 22, 23, 24, 25), HE(26, 27), HFWD(28, 29), HF(30, 31)

#define N_ACTIVE_INPUT_LINKS 12 //Choose TM18_01, TM18_07 or TM18_13 for each TM6 processing stage

static const ap_uint<6> TM18_01[N_ACTIVE_INPUT_LINKS] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 30, 31};
static const ap_uint<6> TM18_07[N_ACTIVE_INPUT_LINKS] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 30, 31};
static const ap_uint<6> TM18_13[N_ACTIVE_INPUT_LINKS] = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

void algo_top(ap_uint<64> link_in[N_INPUT_LINKS], ap_uint<64> link_out[N_OUTPUT_LINKS]);

#endif
