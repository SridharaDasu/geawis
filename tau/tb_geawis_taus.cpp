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

void testZeroPtParticlesIgnored() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 0, 0, 0, ParticleID::HADPLUS);
  runTaus(particles, taus);
  assert(countValid(taus) == 0);

  setParticle(particles[0], 40, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 0, 1, 0, ParticleID::HADMINUS);
  setParticle(particles[2], 0, 0, 1, ParticleID::ELEPLUS);
  setParticle(particles[3], 0, -1, 0, ParticleID::MUMINUS);
  setParticle(particles[4], 0, 0, -1, ParticleID::PHOTON);
  runTaus(particles, taus);
  assert(countValid(taus) == 1);
  assert(taus[0].hwPt == 40);
  assert(taus[0].nProngs == 1);
  assert(taus[0].nPhotons == 0);
}

void testPhiWrapAndFractionalCentroid() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 10, 20, 715, ParticleID::HADPLUS);
  particles[0].hwPt = 10.25;
  setParticle(particles[1], 5, 10, -715, ParticleID::PHOTON);
  particles[1].hwPt = 5.5;
  setParticle(particles[2], 2, 20, -675, ParticleID::HADZERO);
  particles[2].hwPt = 2.25;
  runTaus(particles, taus);
  assert(countValid(taus) == 1);
  assert(taus[0].hwPt == 15.75);
  assert(taus[0].hwEta == 17);
  assert(taus[0].hwPhi == 718);
  assert(taus[0].neutralHadronIso == 2.25);

  particles[0].hwPhi = -715;
  particles[1].hwPhi = 715;
  particles[2].hwPhi = 675;
  runTaus(particles, taus);
  assert(taus[0].hwPhi == -718);
  assert(taus[0].hwPt == 15.75);
  assert(taus[0].totalIso == 2.25);
}

void testConeBoundaries() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 40, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 1, 45, 0, ParticleID::PHOTON); // dr2=2025: signal
  setParticle(particles[2], 2, 45, 9, ParticleID::PHOTON); // dr2=2106: isolation
  setParticle(particles[3], 4, 89, 22, ParticleID::PHOTON); // dr2=8405: included
  setParticle(particles[4], 8, 90, 18, ParticleID::PHOTON); // dr2=8424: excluded
  setParticle(particles[5], 16, 92, 0, ParticleID::PHOTON); // bounding box
  runTaus(particles, taus);
  assert(countValid(taus) == 1);
  assert(taus[0].hwPt == 41);
  assert(taus[0].nPhotons == 1);
  assert(taus[0].photonIso == 6);
}

void testEqualPtSeedAndCandidateOrdering() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 20, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 20, 1, 0, ParticleID::HADMINUS);
  setParticle(particles[2], 20, 0, 1, ParticleID::HADPLUS);
  particles[0].motherid = 15;
  particles[1].motherid = 24;
  setParticle(particles[3], 60, 200, 0, ParticleID::HADMINUS);
  particles[3].motherid = -15;
  runTaus(particles, taus);
  assert(countValid(taus) == 2);
  assert(taus[0].motherid == 15);
  assert(taus[1].motherid == -15);
  assert(taus[0].hwPt == 60 && taus[1].hwPt == 60);
}

void testLargePtSums() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 4095, 0, 0, ParticleID::HADPLUS);
  particles[0].hwPt = 4095.75;
  for (int i = 1; i < NPARTICLES; ++i) {
    setParticle(particles[i], 4095, 1, 0, ParticleID::PHOTON);
    particles[i].hwPt = 4095.75;
  }
  runTaus(particles, taus);
  assert(countValid(taus) == 1);
  assert(taus[0].hwPt == NPARTICLES * 4095.75);
  assert(taus[0].nPhotons == NPARTICLES - 1);
  for (int i = 1; i < NPARTICLES; ++i) particles[i].hwEta = 50;
  runTaus(particles, taus);
  assert(taus[0].hwPt == 4095.75);
  assert(taus[0].photonIso == (NPARTICLES - 1) * 4095.75);
  assert(taus[0].totalIso == taus[0].photonIso);
}

void testEventResetAndAllControlTokens() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 40, 0, 0, ParticleID::HADPLUS);
  runTaus(particles, taus);
  clearParticles(particles);
  for (unsigned bits = 0; bits < 8; ++bits) {
    GEACtrlToken input, output;
    input.start_of_orbit = bits & 1;
    input.dataValid = bits & 2;
    input.frameValid = bits & 4;
    geawis_taus(particles, taus, input, output);
    assert(output.start_of_orbit == input.start_of_orbit);
    assert(output.dataValid == input.dataValid);
    assert(output.frameValid == input.frameValid);
    for (int i = 0; i < NTAUS; ++i) {
      assert(!taus[i].valid && taus[i].hwPt == 0);
      assert(taus[i].hwEta == 0 && taus[i].hwPhi == 0);
      assert(taus[i].motherid == 0 && taus[i].charge == 0);
      assert(taus[i].nProngs == 0 && taus[i].nPhotons == 0);
      assert(taus[i].chargedHadronIso == 0 && taus[i].electronIso == 0);
      assert(taus[i].muonIso == 0 && taus[i].photonIso == 0);
      assert(taus[i].neutralHadronIso == 0 && taus[i].totalIso == 0);
    }
  }
}

void testWideThreeProngRecovery() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 30, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 12, 40, 0, ParticleID::HADPLUS);
  setParticle(particles[2], 10, 0, 40, ParticleID::HADMINUS);
  runTaus(particles, taus);
  assert(countValid(taus) == 1); // smaller cores must not duplicate its prongs
  assert(taus[0].hwPt == 52);
  assert(taus[0].nProngs == 3);
  assert(taus[0].charge == 1);
  assert(taus[0].hwEta == 9 && taus[0].hwPhi == 7);

  // Moving the same cluster across the phi seam preserves its constituents.
  particles[0].hwPhi = 700;
  particles[1].hwPhi = 700;
  particles[2].hwPhi = -700;
  runTaus(particles, taus);
  assert(countValid(taus) == 1);
  assert(taus[0].hwPt == 52 && taus[0].hwPhi == 707);
}

void testWidePhotonRecovery() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 4, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 8, 40, 0, ParticleID::PHOTON);
  runTaus(particles, taus);
  assert(countValid(taus) == 1);
  assert(taus[0].hwPt == 12);
  assert(taus[0].nPhotons == 1);
  assert(taus[0].photonIso == 0);
}

void testCompactCoreRecovery() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 30, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 12, 8, 0, ParticleID::HADPLUS);
  setParticle(particles[2], 10, 0, 8, ParticleID::HADMINUS);
  setParticle(particles[3], 5, 40, 0, ParticleID::HADMINUS);
  runTaus(particles, taus);
  assert(taus[0].hwPt == 52 && taus[0].nProngs == 3);
  assert(taus[0].chargedHadronIso == 5);

  // A two-track wide cone also has a recoverable one-prong compact core.
  clearParticles(particles);
  setParticle(particles[0], 30, 0, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 12, 30, 0, ParticleID::HADMINUS);
  runTaus(particles, taus);
  assert(countValid(taus) == 2);
  assert(taus[0].hwPt == 30 && taus[0].nProngs == 1);
  assert(taus[0].chargedHadronIso == 12);
  assert(taus[1].hwPt == 12 && taus[1].chargedHadronIso == 30);
}

void testOverlappingHypothesisReplacement() {
  Particle_T particles[NPARTICLES];
  Tau_T taus[NTAUS];
  clearParticles(particles);
  setParticle(particles[0], 30, -40, 0, ParticleID::HADPLUS);
  setParticle(particles[1], 40, 40, 0, ParticleID::HADMINUS);
  setParticle(particles[2], 12, 0, 0, ParticleID::PHOTON);
  setParticle(particles[3], 20, 300, 0, ParticleID::HADPLUS);
  runTaus(particles, taus);
  assert(countValid(taus) == 2);
  assert(taus[0].hwPt == 52 && taus[0].charge == -1);
  assert(taus[1].hwPt == 20);
  assert(!taus[2].valid && taus[2].hwPt == 0);

  // Changing ancestry labels must not change clustering or overlap decisions.
  particles[0].motherid = 15;
  particles[1].motherid = -15;
  particles[2].motherid = 321;
  runTaus(particles, taus);
  assert(countValid(taus) == 2);
  assert(taus[0].hwPt == 52 && taus[0].motherid == -15);
  assert(taus[1].hwPt == 20);
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
  testZeroPtParticlesIgnored();
  testPhiWrapAndFractionalCentroid();
  testConeBoundaries();
  testEqualPtSeedAndCandidateOrdering();
  testLargePtSums();
  testEventResetAndAllControlTokens();
  testWideThreeProngRecovery();
  testWidePhotonRecovery();
  testCompactCoreRecovery();
  testOverlappingHypothesisReplacement();

  std::cout << "All geawis_taus tests passed." << std::endl;
  return 0;
}
