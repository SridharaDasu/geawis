#include "geawis.h"
#include <hls_math.h>
#include <cassert>

#ifndef __SYNTHESIS__
#include <cstdio>
#endif

void geawis_stats(Particle_T in_particles[NPARTICLES], Stats &stats, GEACtrlToken token_d, GEACtrlToken& token_q) {
  
  #pragma HLS pipeline II=54

  #pragma HLS ARRAY_PARTITION variable=in_particles complete

  #pragma HLS aggregate variable=in_particles compact=bit
  #pragma HLS aggregate variable=stats compact=bit
  #pragma HLS aggregate variable=token_d compact=bit
  #pragma HLS aggregate variable=token_q compact=bit

  #pragma HLS interface ap_none port=in_particles
  #pragma HLS interface ap_none port=stats
  #pragma HLS interface ap_none port=token_d
  #pragma HLS interface ap_none port=token_q

  Proj_LOOP1:
  stats.maxval = 0;
  stats.minval = 0x3FFF;
  for(int i=0; i<NPARTICLES; ++i) {
    #pragma HLS unroll
    if (in_particles[i].hwPt > stats.maxval) stats.maxval = in_particles[i].hwPt;
    if (in_particles[i].hwPt < stats.minval) stats.minval = in_particles[i].hwPt;
  }
  Proj_LOOP2:
  stats.sum = 0;
  for(int i=0; i<NPARTICLES; ++i) {
    #pragma HLS unroll
    stats.sum += in_particles[i].hwPt;
  }
  Proj_LOOP3:
  // Preserve pt4_t's two fractional bits and factor 144 into 16 * 9.
  ap_uint<34> sumsq_raw = 0;
  for(int i=0; i<NPARTICLES; ++i) {
    #pragma HLS unroll
    ap_uint<14> pt_raw = in_particles[i].hwPt.range();
    ap_uint<28> pt_square_raw = pt_raw * pt_raw;
    sumsq_raw += pt_square_raw >> 2;
  }
  stats.average = stats.sum / NPARTICLES;
  pt2_t avesq = stats.average * stats.average;
  ap_uint<30> sumsq_div_16 = sumsq_raw >> 4;
  ap_uint<30> meansq_raw = sumsq_div_16 / 9;
  pt4_t meansq = 0;
  meansq.range() = meansq_raw;
  stats.variance = meansq - avesq;
  stats.range = stats.maxval - stats.minval;
  
  token_q = token_d;
  return;

}
