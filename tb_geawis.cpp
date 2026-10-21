#include <iostream>
#include <fstream>
#include <cassert>
#include <random>
#include <cmath>
#include <stdexcept>
#include <string>

#include "firmware/geawis.h"

#ifndef NEVENTS
#define NEVENTS 1000
#endif

// Setup random number generator
static std::random_device rd;
static std::mt19937 gen(rd());
// Select uniformly distributed generators for the relevant ranges 
static std::uniform_int_distribution<unsigned int> distrib7FF(0, 0x7FF);
static std::uniform_int_distribution<unsigned int> distribFFF(0, 0xFFF);
static std::uniform_int_distribution<unsigned int> distrib3(0, 0x3);
static std::uniform_int_distribution<unsigned int> distrib6(0, 0x3F);

void getRandomParticles(Particle_T *particles) {

  for(int i=0; i<NPARTICLES; i++){
    particles[i].hwPt = distribFFF(gen);
    particles[i].hwEta = distribFFF(gen);
    particles[i].hwPhi = distrib7FF(gen);
    particles[i].pid.bits = distrib3(gen);
    particles[i].reliso = distrib6(gen);
    particles[i].shoshape = distrib6(gen);
  }
  
}

void getDataFromFile(const char *fileName, Particle_T *particles) {
  static bool first = true;
  static std::ifstream difs;
  if (first) {
    difs.open(fileName);
    if (!difs.is_open()) {
      std::cerr << "Failed to open " << fileName << std::endl;
      exit(1);
    }
    first = false;
  }
  unsigned int ievent, iparticle;
  char comma;
  for(unsigned int i = 0; i < NPARTICLES; i++) {
    difs
      >> ievent >> comma
      >> iparticle >> comma
      >> particles[i].hwPt >> comma
      >> particles[i].hwEta >> comma
      >> particles[i].hwPhi >> comma
      >> particles[i].pid.bits >> comma
      >> particles[i].reliso >> comma
      >> particles[i].shoshape;
  }
}

int main(int argc, char **argv) {

  std::cout << "\n" <<"Start L1 GEA Testbench" << "\n" << std::endl;

  int nevents = NEVENTS;
  bool fileInput = false;
  const char *fileName = nullptr;
  if (argc == 2 || argc == 3) {
    try {
      std::size_t parsedLength = 0;
      nevents = std::stoi(argv[1], &parsedLength);
      if (parsedLength != std::string(argv[1]).length() || nevents < 0) {
        throw std::invalid_argument("event count must be a non-negative integer");
      }
    } catch (const std::exception &e) {
      std::cerr << "Invalid event count: " << e.what() << std::endl;
      return 1;
    }
  } else if (argc != 1) {
    std::cerr << "Usage: " << argv[0] << " [nevents [input-file]]" << std::endl;
    return 1;
  }
  if (argc == 3) {
    fileInput = true;
    fileName = argv[2];
  }

  for (int iEvent=0; iEvent<nevents; ++iEvent) {
    std::cout << "\n" << "Event " << iEvent << " Start" << std::endl;

    Particle_T particles[NPARTICLES];
    if (fileInput) {
      getDataFromFile(fileName, particles);
    } else {
      getRandomParticles(particles);
    }

    GEACtrlToken token_d;
    GEACtrlToken token_q;

    Stats lcl_stats;
    lcl_stats.sum = 0;
    lcl_stats.maxval = 0;
    lcl_stats.minval = 0xFFF;
    pt4_t sumsq = 0;
    for(int i=0; i<NPARTICLES; i++){
      lcl_stats.sum += particles[i].hwPt;
      sumsq += particles[i].hwPt * particles[i].hwPt;
      if (lcl_stats.maxval < particles[i].hwPt) lcl_stats.maxval = particles[i].hwPt;
      if (lcl_stats.minval > particles[i].hwPt) lcl_stats.minval = particles[i].hwPt;
    }
    // Local Stats
    lcl_stats.average = lcl_stats.sum / NPARTICLES;
    lcl_stats.range = lcl_stats.maxval - lcl_stats.minval;
    pt2_t avesq = lcl_stats.average * lcl_stats.average;
    lcl_stats.variance = (sumsq / NPARTICLES) - avesq;
    
    // HLS Stats Calculation
    Stats hls_stats;
    geawis_stats(particles, hls_stats, token_d, token_q);

    if (lcl_stats.sum != hls_stats.sum)
      std::cerr << "Sum (lcl, hls)= " << lcl_stats.sum << "; " << hls_stats.sum << std::endl;
    if (lcl_stats.average != hls_stats.average)
      std::cerr << "Average (lcl, hls)= " << lcl_stats.average << "; " << hls_stats.average << std::endl;
    if (lcl_stats.maxval != hls_stats.maxval)
      std::cerr << "Maxval (lcl, hls)= " << lcl_stats.maxval << "; " << hls_stats.maxval << std::endl;
    if (lcl_stats.minval != hls_stats.minval)
      std::cerr << "Minval (lcl, hls)= " << lcl_stats.minval << "; " << hls_stats.minval << std::endl;
    if (lcl_stats.range != hls_stats.range)
      std::cerr << "Range (lcl, hls)= " << lcl_stats.range << "; " << hls_stats.range << std::endl;
    if (lcl_stats.variance != hls_stats.variance) {
      std::cerr << "Variance (lcl, hls)= " << lcl_stats.variance << "; " << hls_stats.variance << std::endl;
      return 1;
    }

  }

  return 0;

}
