// #include <iostream>
#include "algo_top.h"
#include "topx.h"
#include "data.h"
#include "geawis.h"

void processInputLinks(
  const ap_uint<6> link_numbers[N_ACTIVE_INPUT_LINKS],
  ap_uint<64> link_in[N_INPUT_LINKS], 
  Particle_T in_particles[NPARTICLES]
) {
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=in_particles complete dim=0
#pragma HLS ARRAY_PARTITION variable=link_numbers complete dim=0
  static ap_uint<6> counter[N_ACTIVE_INPUT_LINKS] = {};
#pragma HLS ARRAY_PARTITION variable=counter complete dim=0
  // Loop over active links to select the input data
  static ap_uint<64> mask = 0x0000000000001FFF;
  static ap_uint<64> selected_input[N_ACTIVE_INPUT_LINKS][N_OUT_CANDIDATES];
#pragma HLS ARRAY_PARTITION variable=selected_input complete dim=0
  for(int i = 0; i < N_ACTIVE_INPUT_LINKS; ++i) {
#pragma HLS UNROLL
    ap_uint<64> value = link_in[link_numbers[i]];
    counter[i] = get_topx(counter[i], value, selected_input[i], mask);
  }

  if (counter[0] == 0) {
    // Write code to steal data from 64-bit input to particle data as in the document from Gianluca
    unsigned int ip = 0;
    for(int i = 0; i < N_ACTIVE_INPUT_LINKS; ++i) {
#pragma HLS UNROLL
      for (int j = 0; j < N_OUT_CANDIDATES; j++) {
#pragma HLS UNROLL
        ap_uint<64> value = selected_input[i][j];
        in_particles[ip].hwPt = value.range(13, 0);
        in_particles[ip].hwEta = value.range(23, 14);
        in_particles[ip].hwPhi = value.range(33, 24);
        in_particles[ip].pid.bits = value.range(35, 34);
        ip++;
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
    packed.range(15, 0) = stats.average;
    packed.range(31, 16) = stats.maxval;
    packed.range(47, 32) = stats.minval;
    packed.range(63, 48) = stats.variance;
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
  static ap_uint<6> tm18_counter = 1;

  if (tm18_counter == 1) {
    processInputLinks(TM18_01, link_in, in_particles);
  }
  if (tm18_counter == 6) {
    processInputLinks(TM18_07, link_in, in_particles);
  }
  if (tm18_counter == 13) {
    processInputLinks(TM18_13, link_in, in_particles);
  }

  clock_tick++;
  if (clock_tick == N_INP_CANDIDATES) {
    geawis_stats(in_particles, stats, d, q);
    processOutputLinks(stats, link_out);
    clock_tick = 0;
    if (tm18_counter == 1) {
      tm18_counter = 6;
    }
    else if (tm18_counter == 6) {
      tm18_counter = 13;
    }
    else {
      tm18_counter = 1;
    }
  }
  else {
    for (int i = 0; i < N_OUTPUT_LINKS; i++) {
#pragma HLS UNROLL
      link_out[i] = 0;
    }
  }

}
