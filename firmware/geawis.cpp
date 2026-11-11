#include "geawis.h"
#include <hls_math.h>
#include <cassert>

#ifndef __SYNTHESIS__
#include <cstdio>
#endif

void geawis_stats(Particle_T in_particles[NPARTICLES], Stats &stats, GEACtrlToken token_d, GEACtrlToken& token_q) {
  
  #pragma HLS pipeline II=54

  #pragma HLS ARRAY_PARTITION variable=in_particles complete

  #pragma HLS aggregate variable=in_particles compact=bit
  #pragma HLS aggregate variable=stats compact=bit
  #pragma HLS aggregate variable=token_d compact=bit
  #pragma HLS aggregate variable=token_q compact=bit

  #pragma HLS interface ap_none port=in_particles
  #pragma HLS interface ap_none port=stats
  #pragma HLS interface ap_none port=token_d
  #pragma HLS interface ap_none port=token_q

  Proj_LOOP1:
  stats.maxval = 0;
  stats.minval = 0x3FFF;
  for(int i=0; i<NPARTICLES; ++i) {
    #pragma HLS unroll
    if (in_particles[i].hwPt > stats.maxval) stats.maxval = in_particles[i].hwPt;
    if (in_particles[i].hwPt < stats.minval) stats.minval = in_particles[i].hwPt;
  }
  Proj_LOOP2:
  stats.sum = 0;
  for(int i=0; i<NPARTICLES; ++i) {
    #pragma HLS unroll
    stats.sum += in_particles[i].hwPt;
  }
  Proj_LOOP3:
  // Preserve pt4_t's two fractional bits and factor 144 into 16 * 9.
  ap_uint<34> sumsq_raw = 0;
  for(int i=0; i<NPARTICLES; ++i) {
    #pragma HLS unroll
    ap_uint<14> pt_raw = in_particles[i].hwPt.range();
    ap_uint<28> pt_square_raw = pt_raw * pt_raw;
    sumsq_raw += pt_square_raw >> 2;
  }
  stats.average = stats.sum / NPARTICLES;
  pt2_t avesq = stats.average * stats.average;
  ap_uint<30> sumsq_div_16 = sumsq_raw >> 4;
  ap_uint<30> meansq_raw = sumsq_div_16 / 9;
  pt4_t meansq = 0;
  meansq.range() = meansq_raw;
  stats.variance = meansq - avesq;
  stats.range = stats.maxval - stats.minval;
  
  token_q = token_d;
  return;

}

namespace {

// Try a wider collection cone first, then recover compact cores when extra
// tracks spoil its charge or multiplicity. Radii are 0.20, 0.15 and 0.10.
constexpr int kNTauCones = 3;
constexpr int kSignalConeRadiusSquared[kNTauCones] = {2101, 1182, 528};
constexpr int kIsolationConeRadiusSquared = 8405;
constexpr int kIsolationConeMaxOffset = 91;
static_assert(kIsolationConeMaxOffset * kIsolationConeMaxOffset <=
                  kIsolationConeRadiusSquared &&
              (kIsolationConeMaxOffset + 1) * (kIsolationConeMaxOffset + 1) >
                  kIsolationConeRadiusSquared, "invalid isolation bounding box");
constexpr int kPhiPeriod = Scales::INTPHI_TWOPI;

struct TauCluster {
  unsigned ptRaw;
  int ptEtaOffset;
  int ptPhiOffset;
  unsigned nProngs;
  unsigned nPhotons;
  int charge;
  bool rejected;
  unsigned chargedHadronIso;
  unsigned electronIso;
  unsigned muonIso;
  unsigned photonIso;
  unsigned neutralHadronIso;
  ap_uint<NPARTICLES> constituents;
};

bool isChargedHadron(const Particle_T &particle) {
  return particle.pid.bits == ParticleID::HADPLUS ||
         particle.pid.bits == ParticleID::HADMINUS;
}

bool isElectron(const Particle_T &particle) {
  return particle.pid.bits == ParticleID::ELEPLUS ||
         particle.pid.bits == ParticleID::ELEMINUS;
}

bool isMuon(const Particle_T &particle) {
  return particle.pid.bits == ParticleID::MUPLUS ||
         particle.pid.bits == ParticleID::MUMINUS;
}

int particleCharge(const Particle_T &particle) {
  if (particle.pid.bits == ParticleID::HADPLUS ||
      particle.pid.bits == ParticleID::ELEPLUS ||
      particle.pid.bits == ParticleID::MUPLUS) {
    return 1;
  }
  if (particle.pid.bits == ParticleID::HADMINUS ||
      particle.pid.bits == ParticleID::ELEMINUS ||
      particle.pid.bits == ParticleID::MUMINUS) {
    return -1;
  }
  return 0;
}

int wrappedDeltaPhi(int phi, int seedPhi) {
  int deltaPhi = phi - seedPhi;
  if (deltaPhi > kPhiPeriod / 2) deltaPhi -= kPhiPeriod;
  if (deltaPhi < -kPhiPeriod / 2) deltaPhi += kPhiPeriod;
  return deltaPhi;
}

pt2_t ptFromRaw(unsigned rawPt) {
  // Input pT has two fractional bits; the output has four. Even the sum
  // of NPARTICLES maximum-pT inputs fits in both unsigned and pt2_t.
  static_assert(NPARTICLES * 16383ULL < (1ULL << 22), "pT sum exceeds 22 bits");
  pt2_t pt = 0;
  pt.range() = rawPt << (pt2_t::width - pt2_t::iwidth -
                         (pt_t::width - pt_t::iwidth));
  return pt;
}

void clearTau(Tau_T &tau) {
  tau.hwPt = 0;
  tau.hwEta = 0;
  tau.hwPhi = 0;
  tau.motherid = 0;
  tau.charge = 0;
  tau.nProngs = 0;
  tau.nPhotons = 0;
  tau.chargedHadronIso = 0;
  tau.electronIso = 0;
  tau.muonIso = 0;
  tau.photonIso = 0;
  tau.neutralHadronIso = 0;
  tau.totalIso = 0;
  tau.valid = false;
}

}  // namespace

void geawis_taus(Particle_T in_particles[NPARTICLES], Tau_T taus[NTAUS],
                 GEACtrlToken token_d, GEACtrlToken &token_q) {
#pragma HLS ARRAY_PARTITION variable=in_particles complete
#pragma HLS ARRAY_PARTITION variable=taus complete
#pragma HLS aggregate variable=in_particles compact=bit
#pragma HLS aggregate variable=taus compact=bit
#pragma HLS interface ap_none port=in_particles
#pragma HLS interface ap_none port=taus

  // Decode once per event rather than repeatedly converting fixed-point
  // fields for every seed/constituent pair.
  int eta[NPARTICLES], phi[NPARTICLES], chargeOf[NPARTICLES];
  unsigned ptRaw[NPARTICLES];
  bool hadron[NPARTICLES], electron[NPARTICLES], muon[NPARTICLES];
  bool photon[NPARTICLES];
#pragma HLS ARRAY_PARTITION variable=eta complete
#pragma HLS ARRAY_PARTITION variable=phi complete
#pragma HLS ARRAY_PARTITION variable=chargeOf complete
#pragma HLS ARRAY_PARTITION variable=ptRaw complete
#pragma HLS ARRAY_PARTITION variable=hadron complete
#pragma HLS ARRAY_PARTITION variable=electron complete
#pragma HLS ARRAY_PARTITION variable=muon complete
#pragma HLS ARRAY_PARTITION variable=photon complete
  for (int i = 0; i < NPARTICLES; ++i) {
#pragma HLS UNROLL
    eta[i] = in_particles[i].hwEta.to_int();
    phi[i] = in_particles[i].hwPhi.to_int();
    ptRaw[i] = in_particles[i].hwPt.range().to_uint();
    chargeOf[i] = particleCharge(in_particles[i]);
    hadron[i] = isChargedHadron(in_particles[i]);
    electron[i] = isElectron(in_particles[i]);
    muon[i] = isMuon(in_particles[i]);
    photon[i] = in_particles[i].pid.bits == ParticleID::PHOTON;
  }

  for (int i = 0; i < NTAUS; ++i) {
#pragma HLS UNROLL
    clearTau(taus[i]);
  }

  ap_uint<NPARTICLES> tauConstituents[NTAUS];
#pragma HLS ARRAY_PARTITION variable=tauConstituents complete
  for (int i = 0; i < NTAUS; ++i) {
#pragma HLS UNROLL
    tauConstituents[i] = 0;
  }

  int nTaus = 0;
  for (int seed = 0; seed < NPARTICLES; ++seed) {
    const bool leptonSeed = electron[seed] || muon[seed];
    if (ptRaw[seed] == 0 || (!hadron[seed] && !leptonSeed)) continue;

    TauCluster clusters[kNTauCones] = {};
#pragma HLS ARRAY_PARTITION variable=clusters complete
    for (int particle = 0; particle < NPARTICLES; ++particle) {
#pragma HLS LOOP_FLATTEN off
      if (ptRaw[particle] == 0) continue;
      const int deltaEta = eta[particle] - eta[seed];
      const int deltaPhi = wrappedDeltaPhi(phi[particle], phi[seed]);
      if (deltaEta < -kIsolationConeMaxOffset ||
          deltaEta > kIsolationConeMaxOffset ||
          deltaPhi < -kIsolationConeMaxOffset ||
          deltaPhi > kIsolationConeMaxOffset) continue;
      const int dr2 = deltaEta * deltaEta + deltaPhi * deltaPhi;
      if (dr2 > kIsolationConeRadiusSquared) continue;

      // Share the geometry calculation across the three bounded hypotheses.
      for (int cone = 0; cone < kNTauCones; ++cone) {
#pragma HLS UNROLL
        TauCluster &cluster = clusters[cone];
        if (cluster.rejected) continue;
        if (dr2 < kSignalConeRadiusSquared[cone]) {
          if (hadron[particle]) {
            if (leptonSeed || ++cluster.nProngs > 3 ||
                ptRaw[particle] > ptRaw[seed] ||
                (ptRaw[particle] == ptRaw[seed] && particle < seed)) {
              cluster.rejected = true;
              continue;
            }
            cluster.charge += chargeOf[particle];
          } else if (electron[particle] || muon[particle]) {
            if (particle != seed) {
              cluster.rejected = true;
              continue;
            }
            ++cluster.nProngs;
            cluster.charge += chargeOf[particle];
          } else if (photon[particle]) {
            ++cluster.nPhotons;
          } else {
            continue;
          }
          const int rawPt = ptRaw[particle];
          cluster.ptRaw += rawPt;
          cluster.ptEtaOffset += deltaEta * rawPt;
          cluster.ptPhiOffset += deltaPhi * rawPt;
          cluster.constituents[particle] = 1;
        } else {
          // Particles outside the selected core retain their isolation
          // contribution; no isolation selection is applied here.
          const unsigned rawPt = ptRaw[particle];
          if (hadron[particle]) {
            cluster.chargedHadronIso += rawPt;
          } else if (electron[particle]) {
            cluster.electronIso += rawPt;
          } else if (muon[particle]) {
            cluster.muonIso += rawPt;
          } else if (photon[particle]) {
            cluster.photonIso += rawPt;
          } else {
            cluster.neutralHadronIso += rawPt;
          }
        }
      }
      bool anyActive = false;
      for (int cone = 0; cone < kNTauCones; ++cone) {
#pragma HLS UNROLL
        anyActive |= !clusters[cone].rejected;
      }
      if (!anyActive) break;
    }

    int selectedCone = -1;
    for (int cone = 0; cone < kNTauCones; ++cone) {
#pragma HLS UNROLL
      const TauCluster &cluster = clusters[cone];
      if (selectedCone < 0 && !cluster.rejected &&
          (cluster.nProngs == 1 || cluster.nProngs == 3) &&
          (cluster.charge == 1 || cluster.charge == -1)) {
        selectedCone = cone;
      }
    }
    if (selectedCone < 0) continue;
    const TauCluster &cluster = clusters[selectedCone];

    Tau_T candidate;
    clearTau(candidate);
    candidate.hwPt = ptFromRaw(cluster.ptRaw);
    int tauEta = eta[seed];
    int tauPhi = phi[seed];
    tauEta += cluster.ptEtaOffset / static_cast<int>(cluster.ptRaw);
    tauPhi += cluster.ptPhiOffset / static_cast<int>(cluster.ptRaw);
    if (tauPhi >= kPhiPeriod / 2) tauPhi -= kPhiPeriod;
    if (tauPhi < -kPhiPeriod / 2) tauPhi += kPhiPeriod;
    candidate.hwEta = tauEta;
    candidate.hwPhi = tauPhi;
    candidate.motherid = in_particles[seed].motherid;
    candidate.charge = cluster.charge;
    candidate.nProngs = cluster.nProngs;
    candidate.nPhotons = cluster.nPhotons;
    candidate.valid = true;
    candidate.chargedHadronIso = ptFromRaw(cluster.chargedHadronIso);
    candidate.electronIso = ptFromRaw(cluster.electronIso);
    candidate.muonIso = ptFromRaw(cluster.muonIso);
    candidate.photonIso = ptFromRaw(cluster.photonIso);
    candidate.neutralHadronIso = ptFromRaw(cluster.neutralHadronIso);
    candidate.totalIso = candidate.chargedHadronIso + candidate.electronIso +
                         candidate.muonIso + candidate.photonIso +
                         candidate.neutralHadronIso;

    // Alternate cone sizes can produce overlapping hypotheses even when
    // both seeds are local maxima in their own cores. Keep the higher-pT
    // hypothesis and never count the same signal constituent twice.
    bool overlap[NTAUS];
#pragma HLS ARRAY_PARTITION variable=overlap complete
    bool blocked = false;
    bool hasOverlap = false;
    for (int i = 0; i < NTAUS; ++i) {
#pragma HLS UNROLL
      overlap[i] = i < nTaus &&
                   (tauConstituents[i] & cluster.constituents) != 0;
      if (overlap[i]) {
        hasOverlap = true;
        if (taus[i].hwPt >= candidate.hwPt) blocked = true;
      }
    }
    if (blocked) continue;
    if (hasOverlap) {
      int kept = 0;
      for (int i = 0; i < NTAUS; ++i) {
        if (i < nTaus && !overlap[i]) {
          taus[kept] = taus[i];
          tauConstituents[kept] = tauConstituents[i];
          ++kept;
        }
      }
      nTaus = kept;
      for (int i = 0; i < NTAUS; ++i) {
#pragma HLS UNROLL
        if (i >= nTaus) {
          clearTau(taus[i]);
          tauConstituents[i] = 0;
        }
      }
    }

    int insertAt = 0;
    for (int i = 0; i < NTAUS; ++i) {
#pragma HLS UNROLL
      if (i < nTaus && taus[i].hwPt >= candidate.hwPt) ++insertAt;
    }
    if (insertAt >= NTAUS) continue;

    const int newCount = nTaus < NTAUS ? nTaus + 1 : NTAUS;
    for (int i = NTAUS - 1; i > 0; --i) {
#pragma HLS UNROLL
      if (i > insertAt && i < newCount) {
        taus[i] = taus[i - 1];
        tauConstituents[i] = tauConstituents[i - 1];
      }
    }
    taus[insertAt] = candidate;
    tauConstituents[insertAt] = cluster.constituents;
    nTaus = newCount;
  }

  token_q = token_d;
}
