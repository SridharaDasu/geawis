// #include <iostream>
#include "algo_top.h"
#include "topx.h"
#include "data.h"
#include "geawis.h"

void processInputLinks(ap_uint<64> link_in[N_INPUT_LINKS], Particle_T in_particles[NPARTICLES]) {
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=in_particles complete dim=0
  static bool newEvent = true;
  static const ap_uint<6> *link_numbers = TM18_01;
  static ap_uint<6> counter[N_ACTIVE_INPUT_LINKS];
#pragma HLS ARRAY_PARTITION variable=counter complete dim=0
  if (newEvent) {
    newEvent = false;
    for (int i = 0; i < N_ACTIVE_INPUT_LINKS; i++) {
      counter[i] = 0;
    }
  }
  // Loop over active links to select the input data
  static ap_uint<64> mask = 0x0000000000001FFF;
  static ap_uint<64> selected_input[N_ACTIVE_INPUT_LINKS][N_OUT_CANDIDATES];
#pragma HLS ARRAY_PARTITION variable=selected_input complete dim=0
  for(int i = 0; i < N_ACTIVE_INPUT_LINKS; ++i) {
#pragma HLS UNROLL
    ap_uint<64> value = link_in[link_numbers[i]];
    counter[i] = get_topx(counter[i], value, selected_input[i], mask);
    if (newEvent && counter[i] != 0) {
      // std::cerr << "processInputLinks: counter for " << i << " is not zero as it should be" << std::endl;
      counter[i] = 0;
    }
    else {
      if (counter[i] == 0 ) newEvent = true;
    }
  }
  if (newEvent) {
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
    newEvent = false;
    // Determine the active link set
    if(link_numbers == TM18_01) {
      link_numbers = TM18_07;
    }
    else if(link_numbers == TM18_07) {
      link_numbers = TM18_13;
    }
    else if(link_numbers == TM18_13) {
      link_numbers = TM18_01;
    }
  }
}

void processOutputLinks(Stats &stats, ap_uint<64> link_out[N_OUTPUT_LINKS]) {
#pragma HLS ARRAY_PARTITION variable=link_out complete dim=0
#pragma HLS PIPELINE
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
#pragma HLS PIPELINE
#pragma HLS INTERFACE ap_ctrl_hs port=return

  Particle_T in_particles[NPARTICLES];
  Stats stats;
  GEACtrlToken d;
  GEACtrlToken q;
  static ap_uint<6> counter = 0;
  processInputLinks(link_in, in_particles);
  counter++;
  if(counter == N_INP_CANDIDATES) {
    geawis_stats(in_particles, stats, d, q);
    processOutputLinks(stats, link_out);
    counter = 0;
  }
  else {
    for (int i = 0; i < N_OUTPUT_LINKS; i++) {
#pragma HLS UNROLL
      link_out[i] = 0;
    }
  }

}
