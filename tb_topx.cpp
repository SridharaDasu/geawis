#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>
#include "firmware/topx.h"

int main() {
  const ap_uint<64> mask = 0x1FFF;
  ap_uint<64> result[N_OUT_CANDIDATES] = {};
  unsigned int counter = 0;

  for (int frame = 0; frame < 10; ++frame) {
    std::vector<uint64_t> values;
    for (int i = 0; i < N_INP_CANDIDATES; ++i) {
      uint64_t value = frame % 2 == 0 ? ((frame * 97 + i * 53) % 0x2000) : (i % 7);
      value |= static_cast<uint64_t>((frame + i) & 0x7) << 34;
      values.push_back(value);
      counter = get_topx(counter, value, result, mask);
    }

    std::vector<uint64_t> expected;
    for (uint64_t value : values) {
      expected.push_back(value & static_cast<uint64_t>(mask));
    }
    std::sort(expected.begin(), expected.end(), std::greater<uint64_t>());
    expected.resize(N_OUT_CANDIDATES);

    std::vector<uint64_t> actual;
    for (int i = 0; i < N_OUT_CANDIDATES; ++i) {
      actual.push_back(result[i].to_uint64() & static_cast<uint64_t>(mask));
    }
    std::sort(actual.begin(), actual.end(), std::greater<uint64_t>());

    if (actual != expected) {
      std::cerr << "Top-X mismatch in frame " << frame << std::endl;
      return 1;
    }
    if (counter != 0) {
      std::cerr << "Counter did not wrap at frame boundary" << std::endl;
      return 1;
    }
  }

  std::cout << "Top-X matches software reference for all frames." << std::endl;
  return 0;
}
