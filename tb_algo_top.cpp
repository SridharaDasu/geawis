#include <iostream>
#include <fstream>
#include <cassert>
#include <random>
#include <cmath>

#include "firmware/data.h"
#include "firmware/topx.h"
#include "firmware/algo_top.h"

void getDataFromFile(const char* fileName, int event, int counter, ap_uint<64> link_in[N_INPUT_LINKS]) {
  static std::ifstream difs;  
  if (event = 0) {
    difs.open(fileName);
    if (!difs.is_open()) {
      std::cerr << "Failed to open " << fileName << std::endl;
      exit(1);
    }
  }
  static ap_uint<64> full_link_data[N_ACTIVE_INPUT_LINKS][N_INP_CANDIDATES];
  unsigned int ievent, iparticle;
  char comma;
  // Determine the active link set
  static const ap_uint<6> *link_numbers = TM18_01;
  if(counter == 0) {
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
    int link_counts[N_ACTIVE_INPUT_LINKS];
    for(int i = 0; i < N_ACTIVE_INPUT_LINKS; i++) {
      link_counts[i] = 0;
    }
    for(unsigned int i = 0; i < NPARTICLES; i++) {
      Particle_T particle;
      difs
	>> ievent >> comma
	>> iparticle >> comma
	>> particle.hwPt >> comma
	>> particle.hwEta >> comma
	>> particle.hwPhi >> comma
	>> particle.pid.bits >> comma
	>> particle.reliso >> comma
	>> particle.shoshape;
      // Find the link id based on hwEta
      int iActive;
      float eta = Scales::floatEta(particle.hwEta);
      if (eta < -3.0) {
	iActive = 10;
      }
      else if(eta < -2.5) {
	iActive = 8;
      }
      else if(eta < -1.5) {
	iActive = 6;
      }
      else if(eta < -1.0) {
	iActive = 0;
      }
      else if(eta < -0.5) {
	iActive = 1;
      }
      else if(eta < 0.0) {
	iActive = 2;
      }
      else if(eta < 0.5) {
	iActive = 3;
      }
      else if(eta < 1.0) {
	iActive = 4;
      }
      else if(eta < 1.5) {
	iActive = 5;
      }
      else if(eta < 2.5) {
	iActive = 7;
      }
      else if(eta < 3.0) {
	iActive = 9;
      }
      else if (eta >= 3.0 ) {
	iActive = 11;
      }
      ap_uint<64> coded_value = (particle.pid.bits << 34) + (particle.hwPhi << 24) + (particle.hwEta << 14) + particle.hwPt;
      full_link_data[iActive][link_counts[iActive]] = coded_value;
      if(link_counts[iActive] < (N_INP_CANDIDATES - 1)) link_counts[iActive]++;
    }
  }
  for (int i = 0; i < N_INPUT_LINKS; i++) {
    link_in[i] = 0;
  }
  for (int i = 0; i < N_ACTIVE_INPUT_LINKS; i++) {
    link_in[link_numbers[i]] = full_link_data[i][counter];
  }
  return;
}

int main(int argc, char** argv) {
  try {
    // Get nevents from argument
    int nevents = 10;
    if (argc == 2 || argc == 3) {
      nevents = atoi(argv[1]);
    }
    bool fileInput = false;
    char *fileName;
    std::ifstream difs;
    if (argc == 3) {
      fileInput = true;
      fileName = argv[2];
      difs.open(fileName);
      if (!difs.is_open()) {
	std::cerr << "Failed to open " << fileName << std::endl;
	exit(1);
      }
    }
    // Setup random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    // Select uniformly distributed generators for the relevant ranges 
    static std::uniform_int_distribution<unsigned int> distrib(0, 1000000);
    ap_uint<64> link_in[N_INPUT_LINKS];
    ap_uint<64> link_out[N_OUTPUT_LINKS];
    for (int event = 0; event < nevents; event++) {
      for (int counter = 0; counter < N_INP_CANDIDATES; counter++) {
	if(fileInput) {
	  getDataFromFile(fileName, event, counter, link_in);
	}
	else {
	  for (int i = 0; i < N_INPUT_LINKS; i++) {
	    link_in[i] = distrib(gen); // Use a random input
	  }
	}
	for (int i = 0; i < N_OUTPUT_LINKS; i++) {
	  link_out[i] = 0; // Zero the output links
	}
	algo_top(link_in, link_out);
      }
      for (int i = 0; i < N_OUTPUT_LINKS; i++) {
	std::cout << "Event = " << event << "; Output = " << std::hex << link_out[i] << std::dec << std::endl;
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}
