#include <random>
#include "firmware/topx.h"
#include "firmware/algo_top.h"

int main(int argc, char** argv) {
  try {
    // Get nevents from argument
    int nevents = 10;
    if (argc == 2) {
      nevents = atoi(argv[1]);
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
	for (int i = 0; i < N_INPUT_LINKS; i++) {
	  link_in[i] = distrib(gen); // Use a random input
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
