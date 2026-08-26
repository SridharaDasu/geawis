#ifndef GEAWIS_DATA_H
#define GEAWIS_DATA_H

#include "DataFormats/L1TParticleFlow/interface/datatypes.h"
using namespace l1ct;

#include "ap_int.h"
#include "ap_fixed.h"

#include <hls_math.h>

#define NPARTICLES_POWER 7
#define NPARTICLES 128

class Particle_T {
public:
    pt_t hwPt;
    eta_t hwEta;
    phi_t hwPhi;
    ParticleID pid;
};

struct Stats{
    pt2_t sum;
    pt_t average;
    pt_t maxval;
    pt_t minval;
    pt_t range;
    pt_t variance;
    pt2_t sumaboveave;
    pt2_t sumbelowave;
};

struct GEACtrlToken{
    ap_uint<1> start_of_orbit;
    bool dataValid;
    bool frameValid;
};

#endif

