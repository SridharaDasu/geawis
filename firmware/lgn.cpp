#include "lgn.h"
#include "lgn_thresholds.h"

// Thermometer-encodes one particle's (hwPt, hwEta, hwPhi) against the
// per-feature threshold rows into N_LGN_INPUTS booleans. Row layout and bit
// convention are documented in lgn_thresholds.h.
static void binarize_particles(Particle_T in_particles[NPARTICLES], bool bits[N_LGN_INPUTS]) {
#pragma HLS ARRAY_PARTITION variable=bits complete
  for (int p = 0; p < NPARTICLES; ++p) {
#pragma HLS UNROLL
    float features[LGN_N_FEATURES_PER_PARTICLE] = {
      (float)in_particles[p].hwPt,
      (float)in_particles[p].hwEta,
      (float)in_particles[p].hwPhi,
    };
    for (int f = 0; f < LGN_N_FEATURES_PER_PARTICLE; ++f) {
#pragma HLS UNROLL
      int row = p * LGN_N_FEATURES_PER_PARTICLE + f;
      for (int b = 0; b < LGN_N_THRESHOLD_BITS; ++b) {
#pragma HLS UNROLL
        bits[row * LGN_N_THRESHOLD_BITS + b] = features[f] > LGN_THRESHOLDS[row][b];
      }
    }
  }
}

void lgn_classify(Particle_T in_particles[NPARTICLES], ap_uint<2> &lgn_class) {
#pragma HLS ARRAY_PARTITION variable=in_particles complete

  bool bits[N_LGN_INPUTS];
  ap_uint<7> scores[N_LGN_OUTPUTS];
#pragma HLS ARRAY_PARTITION variable=bits complete
#pragma HLS ARRAY_PARTITION variable=scores complete

  binarize_particles(in_particles, bits);
  lgn_circuit(bits, scores);

  ap_uint<2> best = 0;
  for (int k = 1; k < N_LGN_OUTPUTS; ++k) {
#pragma HLS UNROLL
    if (scores[k] > scores[best]) best = k;
  }
  lgn_class = best;
}
