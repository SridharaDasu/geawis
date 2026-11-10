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

constexpr int kSignalConeRadiusSquared = 1182;
constexpr int kIsolationConeRadiusSquared = 8405;
constexpr int kPhiPeriod = 1440;

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

int deltaR2(const Particle_T &a, const Particle_T &b) {
  const int deltaEta = a.hwEta.to_int() - b.hwEta.to_int();
  int deltaPhi = a.hwPhi.to_int() - b.hwPhi.to_int();
  if (deltaPhi > kPhiPeriod / 2) deltaPhi -= kPhiPeriod;
  if (deltaPhi < -kPhiPeriod / 2) deltaPhi += kPhiPeriod;
  return deltaEta * deltaEta + deltaPhi * deltaPhi;
}

pt2_t particlePt(const Particle_T &particle) {
  return particle.hwPt;
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

  for (int i = 0; i < NTAUS; ++i) {
#pragma HLS UNROLL
    clearTau(taus[i]);
  }

  int nTaus = 0;
  for (int seed = 0; seed < NPARTICLES; ++seed) {
    const bool leptonSeed =
        isElectron(in_particles[seed]) || isMuon(in_particles[seed]);
    if (!isChargedHadron(in_particles[seed]) && !leptonSeed) continue;

    const pt2_t seedPt = particlePt(in_particles[seed]);
    int signalPtEtaOffset = 0;
    int signalPtPhiOffset = 0;
    unsigned signalPtRaw = 0;
    pt2_t signalPt = 0;
    unsigned nProngs = 0;
    int charge = 0;
    bool leptonVeto = false;
    unsigned nPhotons = 0;
    bool hasHigherPtSignalProng = false;
    bool hasOtherSignalTrack = false;
    pt2_t chargedHadronIso = 0;
    pt2_t electronIso = 0;
    pt2_t muonIso = 0;
    pt2_t photonIso = 0;
    pt2_t neutralHadronIso = 0;

    for (int particle = 0; particle < NPARTICLES; ++particle) {
#pragma HLS LOOP_FLATTEN off
      const int dr2 = deltaR2(in_particles[seed], in_particles[particle]);
      if (dr2 < kSignalConeRadiusSquared) {
        if (isChargedHadron(in_particles[particle])) {
          if (leptonSeed) {
            hasOtherSignalTrack = true;
          } else {
            const pt2_t pt = particlePt(in_particles[particle]);
            ++nProngs;
            charge += particleCharge(in_particles[particle]);
            signalPt += pt;
            const int rawPt =
                in_particles[particle].hwPt.range().to_uint();
            signalPtRaw += rawPt;
            signalPtEtaOffset +=
                (in_particles[particle].hwEta.to_int() -
                 in_particles[seed].hwEta.to_int()) *
                rawPt;
            int deltaPhi = in_particles[particle].hwPhi.to_int() -
                           in_particles[seed].hwPhi.to_int();
            if (deltaPhi > kPhiPeriod / 2) deltaPhi -= kPhiPeriod;
            if (deltaPhi < -kPhiPeriod / 2) deltaPhi += kPhiPeriod;
            signalPtPhiOffset += deltaPhi * rawPt;
            if (particle != seed &&
                (pt > seedPt || (pt == seedPt && particle < seed))) {
              hasHigherPtSignalProng = true;
            }
          }
        } else if (isElectron(in_particles[particle]) ||
                   isMuon(in_particles[particle])) {
          if (particle == seed && leptonSeed) {
            ++nProngs;
            charge += particleCharge(in_particles[particle]);
            signalPt += particlePt(in_particles[particle]);
            const int rawPt =
                in_particles[particle].hwPt.range().to_uint();
            signalPtRaw += rawPt;
          } else {
            leptonVeto = true;
            if (leptonSeed) hasOtherSignalTrack = true;
            if (leptonSeed && particle != seed &&
                (particlePt(in_particles[particle]) > seedPt ||
                 (particlePt(in_particles[particle]) == seedPt &&
                  particle < seed))) {
              hasHigherPtSignalProng = true;
            }
          }
        } else if (in_particles[particle].pid.bits == ParticleID::PHOTON) {
          signalPt += particlePt(in_particles[particle]);
          const int rawPt = in_particles[particle].hwPt.range().to_uint();
          signalPtRaw += rawPt;
          signalPtEtaOffset +=
              (in_particles[particle].hwEta.to_int() -
               in_particles[seed].hwEta.to_int()) *
              rawPt;
          int deltaPhi = in_particles[particle].hwPhi.to_int() -
                         in_particles[seed].hwPhi.to_int();
          if (deltaPhi > kPhiPeriod / 2) deltaPhi -= kPhiPeriod;
          if (deltaPhi < -kPhiPeriod / 2) deltaPhi += kPhiPeriod;
          signalPtPhiOffset += deltaPhi * rawPt;
          ++nPhotons;
        }
      } else if (dr2 <= kIsolationConeRadiusSquared) {
        const pt2_t pt = particlePt(in_particles[particle]);
        if (isChargedHadron(in_particles[particle])) {
          chargedHadronIso += pt;
        } else if (isElectron(in_particles[particle])) {
          electronIso += pt;
        } else if (isMuon(in_particles[particle])) {
          muonIso += pt;
        } else if (in_particles[particle].pid.bits == ParticleID::PHOTON) {
          photonIso += pt;
        } else if (in_particles[particle].pid.bits == ParticleID::HADZERO) {
          neutralHadronIso += pt;
        }
      }
    }

    if (hasHigherPtSignalProng || leptonVeto ||
        hasOtherSignalTrack ||
        (nProngs != 1 && nProngs != 3) ||
        (nProngs == 3 && charge != 1 && charge != -1)) {
      continue;
    }

    Tau_T candidate;
    clearTau(candidate);
    candidate.hwPt = signalPt;
    int tauEta = in_particles[seed].hwEta.to_int();
    int tauPhi = in_particles[seed].hwPhi.to_int();
    if (signalPtRaw > 0) {
      tauEta += signalPtEtaOffset / static_cast<int>(signalPtRaw);
      tauPhi += signalPtPhiOffset / static_cast<int>(signalPtRaw);
    }
    if (tauPhi >= kPhiPeriod / 2) tauPhi -= kPhiPeriod;
    if (tauPhi < -kPhiPeriod / 2) tauPhi += kPhiPeriod;
    candidate.hwEta = tauEta;
    candidate.hwPhi = tauPhi;
    candidate.motherid = in_particles[seed].motherid;
    candidate.charge = charge;
    candidate.nProngs = nProngs;
    candidate.nPhotons = nPhotons;
    candidate.valid = true;
    candidate.chargedHadronIso = chargedHadronIso;
    candidate.electronIso = electronIso;
    candidate.muonIso = muonIso;
    candidate.photonIso = photonIso;
    candidate.neutralHadronIso = neutralHadronIso;
    candidate.totalIso = candidate.chargedHadronIso + candidate.electronIso +
                         candidate.muonIso + candidate.photonIso +
                         candidate.neutralHadronIso;

    int insertAt = 0;
    while (insertAt < nTaus && taus[insertAt].hwPt >= candidate.hwPt) {
      ++insertAt;
    }
    if (insertAt >= NTAUS) continue;

    const int newCount = nTaus < NTAUS ? nTaus + 1 : NTAUS;
    for (int i = newCount - 1; i > insertAt; --i) {
      taus[i] = taus[i - 1];
    }
    taus[insertAt] = candidate;
    nTaus = newCount;
  }

  token_q = token_d;
}
