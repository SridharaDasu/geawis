#include "geawis.h"

#include <cassert>
#include <iostream>

namespace {

void clearParticles(Particle_T particles[NPARTICLES]) {
  for (int i = 0; i < NPARTICLES; ++i) {
    particles[i].hwPt = 0;
    particles[i].hwEta = 0;
    particles[i].hwPhi = 0;
    particles[i].pid = ParticleID(ParticleID::HADZERO);
    particles[i].reliso = 0;
    particles[i].motherid = 0;
  }
}

void clearTaus(Tau_T taus[NTAUS]) {
  for (int i = 0; i < NTAUS; ++i) {
    taus[i].hwPt = 0;
    taus[i].hwEta = 0;
    taus[i].hwPhi = 0;
    taus[i].motherid = 0;
    taus[i].charge = 0;
    taus[i].nProngs = 0;
    taus[i].nPhotons = 0;
    taus[i].chargedHadronIso = 0;
    taus[i].electronIso = 0;
    taus[i].muonIso = 0;
    taus[i].photonIso = 0;
    taus[i].neutralHadronIso = 0;
    taus[i].totalIso = 0;
    taus[i].valid = false;
  }
}

void setParticle(Particle_T &p, unsigned pt, int eta, int phi,
                 ParticleID::PID id) {
  p.hwPt = pt;
  p.hwEta = eta;
  p.hwPhi = phi;
  p.pid = id;
  p.reliso = 0;
  p.motherid = 0;
}

void runTaus(Particle_T particles[NPARTICLES], Tau_T taus[NTAUS]) {
  GEACtrlToken token_d;
  GEACtrlToken token_q;
  token_d.start_of_orbit = 0;
  token_d.dataValid = true;
  token_d.frameValid = true;

  clearTaus(taus);
  geawis_taus(particles, taus, token_d, token_q);

  assert(token_q.start_of_orbit == token_d.start_of_orbit);
  assert(token_q.dataValid == token_d.dataValid);
  assert(token_q.frameValid == token_d.frameValid);
}

int countValid(const Tau_T taus[NTAUS]) {
  int n = 0;
  for (int i = 0; i < NTAUS; ++i) {
    if (taus[i].valid) ++n;
  }
  return n;
}

void testOneProngNoPhoton() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 40, 0, 0, ParticleID::HADPLUS);
  particles[0].motherid = 24;
  runTaus(particles, taus);

  assert(countValid(taus) == 1);
  assert(taus[0].valid);
  assert(taus[0].nProngs == 1);
  assert(taus[0].nPhotons == 0);
  assert(taus[0].charge == 1);
  assert(taus[0].hwPt == 40);
  assert(taus[0].motherid == 24);
  assert(taus[0].totalIso == 0);
}

void testOneProngWithPhotons() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 40, 0, 0, ParticleID::HADMINUS);
  setParticle(particles[1], 7, 10, 0, ParticleID::PHOTON);
  setParticle(particles[2], 5, 0, 10, ParticleID::PHOTON);
  runTaus(particles, taus);

  assert(countValid(taus) == 1);
  assert(taus[0].valid);
  assert(taus[0].nProngs == 1);
  assert(taus[0].nPhotons == 2);
  assert(taus[0].charge == -1);
  assert(taus[0].hwPt == 52);
  assert(taus[0].totalIso == 0);
}

void testSignalConeIncludesNearbyPhoton() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 4, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 7, 30, 0, ParticleID::PHOTON);
  runTaus(particles, taus);

  assert(countValid(taus) == 1);
  assert(taus[0].hwPt == 11);
  assert(taus[0].nProngs == 1);
  assert(taus[0].nPhotons == 1);
}

void testLeptonicTauCandidate() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 20, 0, 0, ParticleID::MUPLUS);
  setParticle(particles[1], 3, 10, 0, ParticleID::PHOTON);
  runTaus(particles, taus);

  assert(countValid(taus) == 1);
  assert(taus[0].hwPt == 23);
  assert(taus[0].charge == 1);
  assert(taus[0].nProngs == 1);
}

void testThreeProng() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  // Net charge +1, all within R < 0.15.
  setParticle(particles[0], 30, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 12, 8, 0, ParticleID::HADPLUS);
  setParticle(particles[2], 10, 0, 8, ParticleID::HADMINUS);
  setParticle(particles[3], 4, 6, 6, ParticleID::PHOTON);
  runTaus(particles, taus);

  assert(countValid(taus) == 1);
  assert(taus[0].valid);
  assert(taus[0].nProngs == 3);
  assert(taus[0].nPhotons == 1);
  assert(taus[0].charge == 1);
  assert(taus[0].hwPt == 56);
}

void testThreeProngWrongChargeRejected() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 30, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 12, 8, 0, ParticleID::HADPLUS);
  setParticle(particles[2], 10, 0, 8, ParticleID::HADPLUS);
  runTaus(particles, taus);

  assert(countValid(taus) == 0);
}

void testSignalElectronVeto() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 40, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 8, 10, 0, ParticleID::ELEPLUS);
  runTaus(particles, taus);

  assert(countValid(taus) == 0);
}

void testSignalMuonVeto() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 40, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 8, 10, 0, ParticleID::MUPLUS);
  runTaus(particles, taus);

  assert(countValid(taus) == 0);
}

void testIsolationComponents() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 50, 0, 0, ParticleID::HADPLUS);

  // 50 coordinate counts corresponds to DeltaR about 0.218, safely in
  // the 0.15 < DeltaR <= 0.40 isolation annulus.
  setParticle(particles[1], 3,  50, 0, ParticleID::HADMINUS);
  setParticle(particles[2], 4,  50, 0, ParticleID::ELEPLUS);
  setParticle(particles[3], 5,   0, 50, ParticleID::MUMINUS);
  setParticle(particles[4], 6,   0,-50, ParticleID::PHOTON);
  setParticle(particles[5], 7,  35,35, ParticleID::HADZERO);

  // Outside R=0.4 and therefore ignored by isolation.
  setParticle(particles[6], 100, 100, 0, ParticleID::PHOTON);

  runTaus(particles, taus);

  assert(countValid(taus) == 2);
  assert(taus[0].valid);
  assert(taus[0].chargedHadronIso == 3);
  assert(taus[0].electronIso == 4);
  assert(taus[0].muonIso == 5);
  assert(taus[0].photonIso == 6);
  assert(taus[0].neutralHadronIso == 7);
  assert(taus[0].totalIso == 25);
  assert(taus[0].hwPt == 50);
}

void testDuplicateSeedSuppression() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 25, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 12, 8, 0, ParticleID::HADMINUS);
  setParticle(particles[2], 10, 0, 8, ParticleID::HADPLUS);
  runTaus(particles, taus);

  // Only the leading charged hadron may seed this 3-prong candidate.
  assert(countValid(taus) == 1);
  assert(taus[0].valid);
  assert(!taus[1].valid);
  assert(!taus[2].valid);
}

void testMotherIdDoesNotAffectTauFinding() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 40, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 7, 10, 0, ParticleID::PHOTON);
  particles[0].motherid = 15;
  particles[1].motherid = -15;
  runTaus(particles, taus);

  assert(countValid(taus) == 1);
  const Tau_T expected = taus[0];

  particles[0].motherid = -321;
  particles[1].motherid = 987;
  runTaus(particles, taus);

  assert(countValid(taus) == 1);
  assert(taus[0].hwPt == expected.hwPt);
  assert(taus[0].hwEta == expected.hwEta);
  assert(taus[0].hwPhi == expected.hwPhi);
  assert(taus[0].charge == expected.charge);
  assert(taus[0].nProngs == expected.nProngs);
  assert(taus[0].nPhotons == expected.nPhotons);
  assert(taus[0].chargedHadronIso == expected.chargedHadronIso);
  assert(taus[0].electronIso == expected.electronIso);
  assert(taus[0].muonIso == expected.muonIso);
  assert(taus[0].photonIso == expected.photonIso);
  assert(taus[0].neutralHadronIso == expected.neutralHadronIso);
  assert(taus[0].totalIso == expected.totalIso);
}

void testKeepHighestPtCandidates() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  for (int i = 0; i < NTAUS; ++i) {
    setParticle(particles[i], 11, i * 150 - 900, 0, ParticleID::HADPLUS);
  }
  setParticle(particles[NTAUS], 8, 1500, 0, ParticleID::HADPLUS);
  setParticle(particles[NTAUS + 1], 8, 1530, 0, ParticleID::PHOTON);
  runTaus(particles, taus);

  assert(countValid(taus) == NTAUS);
  assert(taus[0].hwPt == 16);
}

void testFourProngRejected() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);

  setParticle(particles[0], 30, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 12, 8, 0, ParticleID::HADPLUS);
  setParticle(particles[2], 10, 0, 8, ParticleID::HADMINUS);
  setParticle(particles[3],  8, 6, 6, ParticleID::HADMINUS);
  runTaus(particles, taus);

  assert(countValid(taus) == 0);
}

} // namespace

int main() {
  std::cout << "Running geawis_taus test bench..." << std::endl;

  testOneProngNoPhoton();
  testOneProngWithPhotons();
  testSignalConeIncludesNearbyPhoton();
  testLeptonicTauCandidate();
  testThreeProng();
  testThreeProngWrongChargeRejected();
  testSignalElectronVeto();
  testSignalMuonVeto();
  testIsolationComponents();
  testDuplicateSeedSuppression();
  testMotherIdDoesNotAffectTauFinding();
  testKeepHighestPtCandidates();
  testFourProngRejected();

  std::cout << "All geawis_taus tests passed." << std::endl;
  return 0;
}
