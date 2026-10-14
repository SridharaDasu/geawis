#include "topx.h"

unsigned int get_topx(ap_uint<6> call_num, ap_uint<64> value, ap_uint<64> result[N_OUT_CANDIDATES], ap_uint<64> mask) {
#pragma HLS PIPELINE II=1
  if (call_num < N_OUT_CANDIDATES) {
    result[call_num] = value;
  }
  else {
    ap_uint<64> keys[N_OUT_CANDIDATES];
    bool replace_candidate[N_OUT_CANDIDATES];
#pragma HLS ARRAY_PARTITION variable=keys complete
#pragma HLS ARRAY_PARTITION variable=replace_candidate complete
    ap_uint<64> value_key = value & mask;

    for (int i = 0; i < N_OUT_CANDIDATES; ++i) {
#pragma HLS UNROLL
      keys[i] = result[i] & mask;
    }

    for (int i = 0; i < N_OUT_CANDIDATES; ++i) {
#pragma HLS UNROLL
      bool is_minimum = true;
      for (int j = 0; j < N_OUT_CANDIDATES; ++j) {
#pragma HLS UNROLL
        if (j < i) {
          is_minimum = is_minimum && (keys[i] < keys[j]);
        }
        else if (j > i) {
          is_minimum = is_minimum && (keys[i] <= keys[j]);
        }
      }
      replace_candidate[i] = is_minimum && (value_key > keys[i]);
    }

    for (int i = 0; i < N_OUT_CANDIDATES; ++i) {
#pragma HLS UNROLL
      if (replace_candidate[i]) {
        result[i] = value;
      }
    }
  }

  if (call_num == (N_INP_CANDIDATES - 1)) {
    call_num = 0;
  }
  else {
    call_num++;
  }
  return call_num;
}
