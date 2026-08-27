#include <iostream>
#include <cassert>
#include <random>
#include <cmath>

#include "firmware/geawis.h"

#ifndef NEVENTS
#define NEVENTS 1
#endif

int main() {

  std::cout << "\n" <<"Start L1 GEA Testbench" << "\n" << std::endl;

  // Setup random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  
  // Select uniformly distributed generators for the relevant ranges 
  std::uniform_int_distribution<unsigned int> distrib7FF(0, 0x7FF);
  std::uniform_int_distribution<unsigned int> distribFFF(0, 0xFFF);
  std::uniform_int_distribution<unsigned int> distrib3(0, 0x3);
  std::uniform_int_distribution<unsigned int> distrib6(0, 0x3F);
    
  for (int iEvent=0; iEvent<NEVENTS; ++iEvent) {
    std::cout << "\n" << "Event " << iEvent << " Start" << std::endl;

    Particle_T particles[NPARTICLES];
      
    GEACtrlToken token_d;
    GEACtrlToken token_q;

    Stats lcl_stats;
    lcl_stats.sum = 0;
    lcl_stats.maxval = 0;
    lcl_stats.minval = 0xFFF;
    pt2_t sumsq;
    for(int i=0; i<NPARTICLES; i++){
      particles[i].hwPt = distribFFF(gen);
      particles[i].hwEta = distribFFF(gen);
      particles[i].hwPhi = distrib7FF(gen);
      particles[i].pid.bits = distrib3(gen);
      particles[i].reliso = distrib6(gen);
      particles[i].shoshape = distrib6(gen);
      lcl_stats.sum += particles[i].hwPt;
      if (lcl_stats.maxval < particles[i].hwPt) lcl_stats.maxval = particles[i].hwPt;
      if (lcl_stats.minval > particles[i].hwPt) lcl_stats.minval = particles[i].hwPt;
    }
    // Local Stats
    lcl_stats.average = lcl_stats.sum / NPARTICLES;
    lcl_stats.range = lcl_stats.maxval - lcl_stats.minval;
    pt2_t delsq = 0;
    for(int i=0; i<NPARTICLES; i++){
      delsq += (particles[i].hwPt - lcl_stats.average) * (particles[i].hwPt - lcl_stats.average);
    }
    lcl_stats.variance = delsq / NPARTICLES;
    
    // HLS Stats Calculation
    Stats hls_stats;
    geawis_stats(particles, hls_stats, token_d, token_q);

    if (lcl_stats.sum != hls_stats.sum)
      std::cout << "Sum (lcl, hls)= " << lcl_stats.sum << "; " << hls_stats.sum << std::endl;
    if (lcl_stats.average != hls_stats.average)
      std::cout << "Average (lcl, hls)= " << lcl_stats.average << "; " << hls_stats.average << std::endl;
    if (lcl_stats.maxval != hls_stats.maxval)
      std::cout << "Maxval (lcl, hls)= " << lcl_stats.maxval << "; " << hls_stats.maxval << std::endl;
    if (lcl_stats.minval != hls_stats.minval)
      std::cout << "Minval (lcl, hls)= " << lcl_stats.minval << "; " << hls_stats.minval << std::endl;
    if (lcl_stats.range != hls_stats.range)
      std::cout << "Range (lcl, hls)= " << lcl_stats.range << "; " << hls_stats.range << std::endl;
    if (lcl_stats.variance != hls_stats.variance)
      std::cout << "Variance (lcl, hls)= " << lcl_stats.variance << "; " << hls_stats.variance << std::endl;

  }

  return 0;

}
