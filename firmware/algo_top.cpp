// #include <iostream>
#include "algo_top.h"
#include "topx.h"
#include "data.h"
#include "geawis.h"

void processInputLinks(ap_uint<64> link_in[N_INPUT_LINKS], Particle_T in_particles[NPARTICLES]) {
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=in_particles complete dim=0
  static bool newEvent = true;
  const ap_uint<6> *link_numbers;
  static ap_uint<6> counter[N_ACTIVE_INPUT_LINKS];
#pragma HLS ARRAY_PARTITION variable=counter complete dim=0
  if (newEvent) {
    newEvent = false;
    for (int i = 0; i < N_ACTIVE_INPUT_LINKS; i++) {
      counter[i] = 0;
    }
    link_numbers = TM18_01;
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
	in_particles[ip].hwPt = value & 0x3FFF;
	in_particles[ip].hwEta = (value >> 14) & 0x3FF;
	in_particles[ip].hwPhi = (value >> 24) & 0x3FF;
	in_particles[ip].pid.bits = (value >> 34) & 0x3;
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
    link_out[i] = (stats.average) | (stats.maxval << 16) | (stats.minval << 32) | (stats.variance << 48);
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
    /*
    std::cout << "algo_top::counter = " << counter << "; stats = ("
	      << stats.sum << ", "
	      << stats.average << ", "
	      << stats.maxval << ", "
	      << stats.minval << ", "
	      << stats. range << ", "
	      << stats.variance << ")"
	      << std::endl;
    */
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
