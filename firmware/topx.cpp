#include "ap_int.h"
#include "topx.h"


// Function to sort objects in descending order suitable for parallel processing,
// although not very efficient for CPU-based large array sorting for which use std::sort

void sortDescending(ap_uint<64> array[], int size, ap_uint<64> mask) {
  // Loop bounds must be static/constant for hardware unrolling
  for (int i = 1; i < size; i++) {
    ap_uint<64> value = array[i];
    int j = i - 1;

    // Move elements that are smaller than value to one position ahead
    while (j >= 0 && ((array[j] & mask) < (value & mask))) {
        array[j + 1] = array[j];
        j = j - 1;
      }
      array[j + 1] = value;
      }
}


// This function needs to know the call number so that result can be cleared on call_num=0
// This function handles a single input value per call
// Returns updated call number

unsigned int get_topx(ap_uint<6> call_num, ap_uint<64> value, ap_uint<64> result[N_OUT_CANDIDATES], ap_uint<64> mask) {
#pragma HLS PIPELINE II=1
  if (call_num < N_OUT_CANDIDATES) {
    result[call_num] = value;
  }
  else if (call_num < (N_INP_CANDIDATES - 1)) {
    sortDescending(result, N_OUT_CANDIDATES, mask);
    if ((value & mask) > (result[N_OUT_CANDIDATES - 1] & mask)) {
      result[N_OUT_CANDIDATES - 1] = value;
      sortDescending(result, N_OUT_CANDIDATES, mask);
    }
  }
  else if ((value & mask) > (result[N_OUT_CANDIDATES - 1] & mask)) {
    result[N_OUT_CANDIDATES - 1] = value;
  }
  if (call_num == (N_INP_CANDIDATES - 1)) {
    call_num = 0;
  }
  else {
    call_num++;
  }
  return call_num;
}
