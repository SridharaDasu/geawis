// #include <iostream>
#include "algo_top.h"
#include "topx.h"
#include "data.h"
#include "geawis.h"

void processInputLinks(
  ap_uint<2> link_group,
  ap_uint<64> link_in[N_INPUT_LINKS], 
  Particle_T in_particles[NPARTICLES]
) {
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=in_particles complete dim=0
  static ap_uint<6> counter[N_ACTIVE_INPUT_LINKS] = {};
#pragma HLS ARRAY_PARTITION variable=counter complete dim=0
  // Loop over active links to select the input data
  static ap_uint<64> mask = 0x0000000000003FFF;
  static ap_uint<64> selected_input[N_ACTIVE_INPUT_LINKS][N_OUT_CANDIDATES];
#pragma HLS ARRAY_PARTITION variable=selected_input complete dim=0
  for(int i = 0; i < N_ACTIVE_INPUT_LINKS; ++i) {
#pragma HLS UNROLL
    ap_uint<6> link_number = TM18_01[i];
    if (link_group == 1) {
      link_number = TM18_07[i];
    }
    else if (link_group == 2) {
      link_number = TM18_13[i];
    }
    ap_uint<64> value = link_in[link_number];
    counter[i] = get_topx(counter[i], value, selected_input[i], mask);
  }

  if (counter[0] == 0) {
    for(int i = 0; i < N_ACTIVE_INPUT_LINKS; ++i) {
#pragma HLS UNROLL
      for (int j = 0; j < N_OUT_CANDIDATES; j++) {
#pragma HLS UNROLL
        ap_uint<64> value = selected_input[i][j];
        unsigned int ip = i * N_OUT_CANDIDATES + j;
        in_particles[ip].hwPt = value.range(13, 0);
        in_particles[ip].hwEta = value.range(23, 14);
        in_particles[ip].hwPhi = value.range(33, 24);
        in_particles[ip].pid.bits = value.range(36, 34);
      }
    }
  }
}

void processOutputLinks(Stats &stats, ap_uint<64> link_out[N_OUTPUT_LINKS]) {
#pragma HLS ARRAY_PARTITION variable=link_out complete dim=0
#pragma HLS PIPELINE II=1
  for (int i = 0; i < N_OUTPUT_LINKS; i++) {
#pragma HLS UNROLL
    ap_uint<64> packed = 0;
    packed.range(13, 0) = stats.average;
    packed.range(27, 14) = stats.maxval;
    packed.range(41, 28) = stats.minval;
    packed.range(55, 42) = stats.variance;
    link_out[i] = packed;
  }
}

void algo_top(ap_uint<64> link_in[N_INPUT_LINKS], ap_uint<64> link_out[N_OUTPUT_LINKS]){
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=link_out complete dim=0
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE ap_ctrl_hs port=return

  Particle_T in_particles[NPARTICLES];
  Stats stats;
  GEACtrlToken d = {0, true, true};
  GEACtrlToken q = {0, false, false};
  static ap_uint<6> clock_tick = 0;
  static ap_uint<2> tm18_counter = 0;

  processInputLinks(tm18_counter, link_in, in_particles);
  clock_tick++;
  if (clock_tick == N_INP_CANDIDATES) {
    geawis_stats(in_particles, stats, d, q);
    processOutputLinks(stats, link_out);
    clock_tick = 0;
    if (tm18_counter == 2) {
      tm18_counter = 0;
    }
    else {
      tm18_counter++;
    }
  }
  else {
    for (int i = 0; i < N_OUTPUT_LINKS; i++) {
#pragma HLS UNROLL
      link_out[i] = 0;
    }
  }

}
