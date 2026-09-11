#include <random>
#include "firmware/topx.h"

int main(int argc, char** argv) {
  try {
    // Setup random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    // Select uniformly distributed generators for the relevant ranges 
    static std::uniform_int_distribution<unsigned int> distrib(0, 1000000);
    std::vector<unsigned int> stream;
    for (int i = 0; i < (N_INP_CANDIDATES * 10); i++) {
      stream.push_back(distrib(gen));
    }
    unsigned int result[N_OUT_CANDIDATES];
    unsigned int counter = 0;
    for (int c = 0; c < stream.size(); c++) {
      unsigned int value = stream[c];
      counter = get_topx(counter, value, result);
      if ((counter % N_INP_CANDIDATES) == 0) {
	std::cout << "Reached N calls! The top X numbers are: ";
	for (int i = 0; i < N_OUT_CANDIDATES ; i++) std::cout << result[i] << " "; std::cout << std::endl;
      } else {
	std::cout << "Processed " << c << " " << counter << " = " << value << ", waiting for " << N_INP_CANDIDATES << " calls..." << std::endl;
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}
