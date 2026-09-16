#ifndef TOPX_H
#define TOPX_H
#include "ap_int.h"

#define N_INP_CANDIDATES 54
#define N_OUT_CANDIDATES 12

unsigned int get_topx(ap_uint<6> call_num, ap_uint<64> value, ap_uint<64> result[N_OUT_CANDIDATES], ap_uint<64> mask=0xFFFFFFFFFFFFFFFF);

#endif
