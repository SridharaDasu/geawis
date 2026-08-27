#include "geawis.h"
#include <hls_math.h>
#include <cassert>
#ifndef __SYNTHESIS__
#include <cstdio>
#endif

void geawis_stats(Particle_T in_particles[NPARTICLES], Stats &stats, GEACtrlToken token_d, GEACtrlToken& token_q) {
  
  #pragma HLS pipeline

  #pragma HLS ARRAY_PARTITION variable=in_particles complete

  #pragma HLS aggregate variable=in_particles compact=bit
  #pragma HLS aggregate variable=stats compact=bit
  #pragma HLS aggregate variable=token_d compact=bit
  #pragma HLS aggregate variable=token_q compact=bit

  #pragma HLS interface ap_none port=in_particles
  #pragma HLS interface ap_none port=stats
  #pragma HLS interface ap_none port=token_d
  #pragma HLS interface ap_none port=token_q

  Proj_LOOP:
  stats.maxval = 0;
  stats.minval = 0xFFF;
  stats.sum = 0;
  pt2_t sumsq = 0;
  for(int i=0; i<NPARTICLES; ++i) {
    #pragma HLS unroll
    stats.sum += in_particles[i].hwPt;
    sumsq += in_particles[i].hwPt * in_particles[i].hwPt;
    if (in_particles[i].hwPt > stats.maxval) stats.maxval = in_particles[i].hwPt;
    if (in_particles[i].hwPt < stats.minval) stats.minval = in_particles[i].hwPt;
  }
  stats.average = stats.sum >> NPARTICLES_POWER;
  stats.variance = (sumsq >> NPARTICLES_POWER) - (stats.average * stats.average);
  stats.range = stats.maxval - stats.minval;
  
  token_q = token_d;
  return;

}
