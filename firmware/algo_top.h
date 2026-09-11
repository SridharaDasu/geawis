#ifndef _ALGO_TOP_H_
#define _ALGO_TOP_H_

#include "ap_int.h"

#define N_INPUT_LINKS  36
#define N_OUTPUT_LINKS  1

void algo_top(ap_uint<576> link_in[N_INPUT_LINKS], ap_uint<576> link_out[N_OUTPUT_LINKS]);

#endif
