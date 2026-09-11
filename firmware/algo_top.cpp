#include "algo_top.h"
#include "data.h"
#include "geawis.h"

void processInputLinks(ap_uint<64> link_in[N_INPUT_LINKS], Particle_T in_particles[NPARTICLES]) {
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=in_particles complete dim=0
#pragma HLS latency min=54
  const char TM = 6;
  const char pt_lo_bit = 0;
  const char pt_hi_bit = 13;
  const char eta_lo_bit = 14;
  const char eta_hi_bit = 25;
  const char phi_lo_bit = 26;
  const char phi_hi_bit = 36;
  const char pid_lo_bit = 37;
  const char pid_hi_bit = 39;
  for(int i = 0; i < NPARTICLES; ++i) {
    // Write code to steal data from 64-bit input to particle data as in the document from Gianluca
  }
}

void algo_top(ap_uint<64> link_in[N_INPUT_LINKS], ap_uint<64> link_out[N_OUTPUT_LINKS]){
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=link_out complete dim=0
#pragma HLS PIPELINE II=54
#pragma HLS INTERFACE ap_ctrl_hs port=return

  Particle_T in_particles[NPARTICLES];
  
  processInputLinks(link_in, in_particles);
  
  geawis_stats(in_particles, stats);

  processOutLinks(stats, link_out);

}
