#include "topx.h"

// Sorted top-N selection via a local, neighbor-only insertion-point check.
//
// Invariant: `result[]` is sorted descending (by `value & mask`) at all
// times. Because of that invariant, `ge[i] = (result[i] >= value)` is
// monotonic across i (true...true, false...false) -- the insertion point is
// exactly where it flips from true to false. That flip can be detected with
// a single neighbor comparison per slot (`ge[i-1] && !ge[i]`), with no
// global reduction (sum/rank) needed at all: depth is compare -> AND (with
// one neighbor) -> mux, independent of N_OUT_CANDIDATES.
//
// This replaces an earlier version that computed a numeric rank via an
// adder-tree (`rank = sum(ge[i])`) and then re-compared each slot's index
// against that number -- functionally correct, but two serial reduction
// stages (an adder-tree, then a magnitude compare) where this formulation
// needs neither: full `algo_top` csynth went from an estimated 3.159ns
// critical path (adder-tree version) to be re-measured with this version.
//
// `result[]` is a `static` array in the caller (firmware/algo_top.cpp),
// reused across consecutive N_INP_CANDIDATES-sized blocks, so on call_num
// == 0 the (possibly stale, previous-block) held values must not be
// compared against -- the call_num==0 override is folded into the final
// select stage below rather than a separate reset pass, to keep it off the
// critical path.
unsigned int get_topx(ap_uint<6> call_num, ap_uint<64> value, ap_uint<64> result[N_OUT_CANDIDATES], ap_uint<64> mask) {
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=result complete

  ap_uint<1> ge[N_OUT_CANDIDATES];
#pragma HLS ARRAY_PARTITION variable=ge complete
  for (int i = 0; i < N_OUT_CANDIDATES; i++) {
#pragma HLS UNROLL
    ge[i] = ((result[i] & mask) >= (value & mask));
  }

  ap_uint<1> is_insert_point[N_OUT_CANDIDATES];
#pragma HLS ARRAY_PARTITION variable=is_insert_point complete
  for (int i = 0; i < N_OUT_CANDIDATES; i++) {
#pragma HLS UNROLL
    is_insert_point[i] = (i == 0) ? ap_uint<1>(!ge[0]) : ap_uint<1>(ge[i - 1] && !ge[i]);
  }

  ap_uint<64> new_result[N_OUT_CANDIDATES];
#pragma HLS ARRAY_PARTITION variable=new_result complete
  for (int i = 0; i < N_OUT_CANDIDATES; i++) {
#pragma HLS UNROLL
    ap_uint<64> normal_case = ge[i] ? result[i] : (is_insert_point[i] ? value : result[i - 1]);
    ap_uint<64> reset_case = (i == 0) ? value : ap_uint<64>(0);
    new_result[i] = (call_num == 0) ? reset_case : normal_case;
  }

  for (int i = 0; i < N_OUT_CANDIDATES; i++) {
#pragma HLS UNROLL
    result[i] = new_result[i];
  }

  if (call_num == (N_INP_CANDIDATES - 1)) {
    call_num = 0;
  } else {
    call_num++;
  }
  return call_num;
}
