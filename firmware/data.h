#ifndef GEAWIS_DATA_H
#define GEAWIS_DATA_H

#include "DataFormats/L1TParticleFlow/interface/datatypes.h"
using namespace l1ct;

#include "ap_int.h"
#include "ap_fixed.h"
typedef ap_ufixed<34, 32, AP_TRN, AP_SAT> pt4_t;

#include <hls_math.h>

#include "algo_top.h"
#include "topx.h"

#define NPARTICLES (N_ACTIVE_INPUT_LINKS * N_OUT_CANDIDATES)
#define NTAUS 12

class Particle_T {
public:
  pt_t hwPt;
  glbeta_t hwEta;
  glbphi_t hwPhi;
  ParticleID pid;
  rel_iso_t reliso;
  ap_int<16> motherid;
};

struct Stats{
  pt2_t sum;
  pt_t average;
  pt_t maxval;
  pt_t minval;
  pt_t range;
  pt_t variance;
};

struct Tau_T {
  pt2_t hwPt;
  glbeta_t hwEta;
  glbphi_t hwPhi;
  ap_int<16> motherid;
  ap_int<3> charge;
  ap_uint<2> nProngs;
  ap_uint<8> nPhotons;
  pt2_t chargedHadronIso;
  pt2_t electronIso;
  pt2_t muonIso;
  pt2_t photonIso;
  pt2_t neutralHadronIso;
  pt2_t totalIso;
  bool valid;
};

struct GEACtrlToken{
  ap_uint<1> start_of_orbit;
  bool dataValid;
  bool frameValid;
};

#endif
