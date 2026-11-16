// Regression test for the gate network generated into firmware/lgn.cpp:
// checks it is bit-exact with lgn_stuff/LogicModelTiny_20261102-180844/
// circuit.c, the reference C implementation emitted directly by the
// training/compilation pipeline that scripts/generate_lgn_firmware.py reads
// circuit.json from. This does not test binarize_particles()/lgn_classify()
// against real physics -- only that the gate network was transcribed
// correctly (see lgn.h for the caveat on the assumed input feature
// ordering).
#include <iostream>
#include "ap_int.h"

// Sizes for this specific compiled model (lgn_stuff/LogicModelTiny_20261102-
// 180844/circuit.json); keep in sync with it if the model directory above
// changes. lgn_circuit() itself is generated into firmware/lgn.cpp -- it has
// no header since it's not part of the firmware's public API (lgn.h/
// lgn_classify() is), so this testbench forward-declares it directly to
// exercise it in isolation.
#define N_LGN_INPUTS 1728
#define N_LGN_OUTPUTS 3
extern void lgn_circuit(bool in[N_LGN_INPUTS], ap_uint<7> out[N_LGN_OUTPUTS]);

// Reference implementation, compiled alongside this testbench (see
// run_csim_lgn.tcl); not synthesized.
extern "C" void circuit(const bool in[N_LGN_INPUTS], float out[N_LGN_OUTPUTS]);

int main() {
  const float tau = 4.636809247747852f;
  const int n_trials = 2000;
  int mismatches = 0;

  unsigned int lcg_state = 12345;
  auto next_bit = [&lcg_state]() {
    lcg_state = lcg_state * 1103515245u + 12345u;
    return (bool)((lcg_state >> 16) & 1u);
  };

  for (int t = 0; t < n_trials; ++t) {
    bool in[N_LGN_INPUTS];
    for (int i = 0; i < N_LGN_INPUTS; ++i) in[i] = next_bit();

    float ref_out[N_LGN_OUTPUTS];
    circuit(in, ref_out);

    ap_uint<7> out[N_LGN_OUTPUTS];
    lgn_circuit(in, out);

    for (int k = 0; k < N_LGN_OUTPUTS; ++k) {
      float ref_raw = ref_out[k] * tau;
      float my_raw = (float)(unsigned)out[k];
      if (my_raw < ref_raw - 0.01f || my_raw > ref_raw + 0.01f) {
        std::cerr << "Mismatch trial " << t << " class " << k
                  << ": reference=" << ref_raw << " got=" << my_raw << std::endl;
        ++mismatches;
      }
    }
  }

  if (mismatches != 0) {
    std::cerr << mismatches << " mismatches over " << n_trials << " trials" << std::endl;
    return 1;
  }
  std::cout << "lgn_circuit matches the reference circuit.c for all " << n_trials << " trials." << std::endl;
  return 0;
}
