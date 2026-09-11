#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

#include "topx.h"


// Function to sort objects in descending order suitable for parallel processing,
// although not very efficient for CPU-based large array sorting for which use std::sort

void sortDescending(unsigned int array[], int size) {
    // Loop bounds must be static/constant for hardware unrolling
    for (int i = 1; i < size; i++) {
        unsigned int value = array[i];
        int j = i - 1;

        // Move elements that are smaller than value to one position ahead
        while (j >= 0 && array[j] < value) {
            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = value;
    }
}

// This function needs to know the call number so that result can be cleared on call_num=0
// This function handles a single input value per call
// Returns updated call number

unsigned int get_topx(unsigned int call_num, unsigned int value, unsigned int result[N_OUT_CANDIDATES]) {
  if (call_num < N_OUT_CANDIDATES) {
    result[call_num] = value;
  }
  else {
    sortDescending(result, N_OUT_CANDIDATES);
    if (value > result[N_OUT_CANDIDATES - 1]) {
      result[N_OUT_CANDIDATES - 1] = value;
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
