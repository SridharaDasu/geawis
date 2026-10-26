#include "ap_int.h"
#include "topx_rank.h"

// Rank-based parallel insertion, replacing the sequential insertion-sort
// chain in topx.cpp's get_topx/sortDescending.
//
// Invariant: `result[]` is sorted descending (by `value & mask`) at all
// times. For a new value V, its correct insertion index equals the number
// of currently-held entries that are >= V (those stay ahead, unchanged) --
// this can be computed with N_OUT_CANDIDATES independent parallel
// comparators plus an adder-tree (not a sequential chain), then the array
// update is a single parallel mux layer keyed only on that rank, with no
// cross-position data dependency. Net depth ~log2(N_OUT_CANDIDATES)+1,
// versus the current code's O(N_OUT_CANDIDATES) sequential dependency chain
// (doubled, since the current code calls sortDescending up to twice per
// cycle).
//
// `result[]` is a `static` array in the caller (firmware/algo_top.cpp),
// reused across consecutive N_INP_CANDIDATES-sized blocks, so on call_num
// == 0 the (possibly stale, previous-block) held values must not be
// compared against -- this mirrors how the original get_topx's first
// N_OUT_CANDIDATES calls bypass comparison entirely and just overwrite.
// Validated (0 mismatches vs. brute-force top-K ground truth, including
// across 5000 consecutive blocks with carried-over state) in
// /scratchpad/sortproto/rank_topx_multiblock.cpp before porting here.
unsigned int get_topx_rank(ap_uint<6> call_num, ap_uint<64> value, ap_uint<64> result[N_OUT_CANDIDATES], ap_uint<64> mask) {
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=result complete

  // Compare directly against `result[]` (the possibly-stale previous-block
  // values when call_num==0) rather than pre-zeroing into a `current[]`
  // intermediate first -- that reset used to sit as its own mux stage in
  // series *before* the comparator on every single cycle (measured 0.411ns
  // of the critical path), even though it only matters on 1 cycle out of
  // 54. Whatever `rank`/`ge[]` computes when call_num==0 is discarded below
  // anyway, so it's fine for it to be "wrong" (based on stale data) here --
  // the call_num==0 override is folded into the *final* select instead, the
  // same pipeline stage as the existing 3-way mux rather than an extra one.
  //
  // Split into (a) independent parallel compares and (b) a plain reduction
  // sum over 0/1 terms -- written as a flat accumulate loop (no per-term
  // control flow) so HLS's expression balancer builds a log-depth adder
  // tree. The equivalent "if (cond) rank++;" form looked parallel (it's
  // unrolled) but is a true sequential dependency chain in the C semantics
  // (each += depends on the previous), which defeats tree-balancing --
  // caught via the isolated csynth comparison (see RUNBOOK.md 3b).
  ap_uint<1> ge[N_OUT_CANDIDATES];
#pragma HLS ARRAY_PARTITION variable=ge complete
  for (int i = 0; i < N_OUT_CANDIDATES; i++) {
#pragma HLS UNROLL
    ge[i] = ((result[i] & mask) >= (value & mask));
  }
  ap_uint<4> rank = 0;
  for (int i = 0; i < N_OUT_CANDIDATES; i++) {
#pragma HLS UNROLL
    rank += ge[i];
  }

  ap_uint<64> new_result[N_OUT_CANDIDATES];
#pragma HLS ARRAY_PARTITION variable=new_result complete
  for (int i = 0; i < N_OUT_CANDIDATES; i++) {
#pragma HLS UNROLL
    ap_uint<64> normal_case = (i < rank) ? result[i] : (i == rank) ? value : result[i - 1];
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
