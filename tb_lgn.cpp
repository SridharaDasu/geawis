// Regression test for firmware/lgn_circuit.cpp: checks it is bit-exact with
// lgn_stuff/LogicModelTiny_20261102-180844/circuit.c, the reference C
// implementation emitted directly by the training/compilation pipeline that
// scripts/generate_lgn_firmware.py reads circuit.json from. This does not
// test binarize_particles()/lgn_classify() against real physics -- only that
// the gate network was transcribed correctly (see lgn.h for the caveat on
// the assumed input feature ordering).
#include <iostream>
#include "firmware/lgn_circuit.h"

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
