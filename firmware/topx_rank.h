#ifndef TOPX_RANK_H
#define TOPX_RANK_H
#include "ap_int.h"
#include "topx.h"  // reuses N_INP_CANDIDATES / N_OUT_CANDIDATES

// Prototype replacement for get_topx (see firmware/topx.cpp): instead of a
// sequential insertion-sort chain (deep, data-dependent), compute the new
// candidate's insertion rank via parallel comparators + a count (shallow,
// ~log2(N_OUT_CANDIDATES) depth), then insert via a single parallel mux
// layer. Same call signature/semantics as get_topx, so it can be swapped in
// at the call site with no other changes, once validated.
unsigned int get_topx_rank(ap_uint<6> call_num, ap_uint<64> value, ap_uint<64> result[N_OUT_CANDIDATES], ap_uint<64> mask=0xFFFFFFFFFFFFFFFF);

#endif
