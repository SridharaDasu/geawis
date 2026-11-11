#include "TauAncestry.h"
#include "UltraFastSim.h"
#include "Pythia8/Pythia.h"
#include "geawis/firmware/data.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace {

void check(bool condition, const char *message) {
  if (!condition) throw std::runtime_error(message);
}

// Independent oracle: walk the original mother graph, not daughter lists.
// This must run BEFORE mother2 is repurposed as a PDG ID.
int closestTau(const Pythia8::Event &event, int particle) {
  std::vector<int> pending = event[particle].motherList();
  std::set<int> visited;
  while (!pending.empty()) {
    const int mother = pending.back();
    pending.pop_back();
    if (mother <= 0 || mother >= event.size() ||
        !visited.insert(mother).second) continue;
    if (std::abs(event[mother].id()) == 15) return mother;
    const auto parents = event[mother].motherList();
    pending.insert(pending.end(), parents.begin(), parents.end());
  }
  return 0;
}

void testSyntheticHistory(Pythia8::ParticleData &particleData) {
  Pythia8::Event event;
  event.init("tau ancestry regression", &particleData);
  event.reset();
  // Entries 1 and 2 are beams; decays start at 3.
  const int ids[] = {2212, 2212, 15, -15, 15, 22, -213, 16,
                     -211, 111, 22, 22, 11, -16, 113, 211, -211};
  const int mothers[] = {0, 0, 0, 0, 3, 3, 5, 5, 7, 7, 10, 10,
                         4, 4, 0, 15, 15};
  for (unsigned i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
    event.append(ids[i], 1, mothers[i], mothers[i], 0, 0, 0, 0,
                 10., 0., 0., 10.);
  }
  for (int i : {3, 4, 5, 7, 10, 15}) event[i].statusNeg();
  event[3].daughters(5, 6);   // tau -> tau copy + radiation
  event[5].daughters(7, 8);   // tau -> rho + neutrino
  event[7].daughters(9, 10);  // rho -> charged pion + pi0
  event[10].daughters(11, 12);// pi0 -> two photons
  event[4].daughters(13, 14); // opposite-sign tau
  event[15].daughters(16, 17);// unrelated mother at index 15

  const Pythia8::Event original = event;
  geawis::storeTauAncestorsInMother2(event);
  for (int i = 0; i < event.size(); ++i) {
    const int ancestor = closestTau(original, i);
    const int expected = ancestor ? original[ancestor].id() : 0;
    check(event[i].mother2() == expected, "synthetic ancestry tag mismatch");
    check(event[i].mother1() == original[i].mother1(), "mother1 changed");
    check(event[i].daughter1() == original[i].daughter1() &&
          event[i].daughter2() == original[i].daughter2(), "daughters changed");
  }
  check(event[16].mother2() == 0, "background index 15 became a tau tag");
  check(event[12].mother2() == 15, "last photon descendant was missed");
  check(event[13].mother2() == -15, "negative tag was lost");

  const auto pileup = original[16];
  geawis::appendUntaggedPileup(event, pileup);
  check(event.back().mother1() == 0 && event.back().mother2() == 0,
        "pileup retained foreign parent indices");
  check(pileup.mother2() == 15, "pileup source was mutated");

  TParticle rootParticle;
  rootParticle.SetLastMother(-15);
  Particle_T firmwareParticle;
  firmwareParticle.motherid = rootParticle.GetSecondMother();
  check(firmwareParticle.motherid == -15, "ROOT/firmware negative tag mismatch");
}

bool selected(const Pythia8::Particle &particle) {
  return particle.isFinal() && particle.isVisible() &&
         particle.pT() > 1. && std::abs(particle.eta()) < 5.;
}

void compareMomentum(double actual, double expected) {
  check(std::abs(actual - expected) < 1.e-8 * (1. + std::abs(expected)),
        "visible-tau momentum differs from final-descendant sum");
}

} // namespace

int main(int argc, char **argv) {
  try {
    const std::string cards = argc > 1 ? argv[1] : "hllhc-z-tau.cards";
    const int nEvents = argc > 2 ? std::stoi(argv[2]) : 1000;
    const double pileupMean = argc > 3 ? std::stod(argv[3]) : 25.;
    check(nEvents > 0 && std::isfinite(pileupMean) && pileupMean >= 0.,
          "expected a positive event count and nonnegative pileup mean");

    Pythia8::Pythia hard;
    check(hard.readFile(cards), "could not read cards");
    hard.readString("Print:quiet = on");
    hard.readString("Random:setSeed = on");
    hard.readString("Random:seed = 12345");
    check(hard.init(), "hard-process initialization failed");
    testSyntheticHistory(hard.particleData);

    Pythia8::Pythia pileup;
    pileup.readString("Print:quiet = on");
    pileup.readString("SoftQCD:all = on");
    pileup.readString("Beams:idA = 2212");
    pileup.readString("Beams:idB = 2212");
    pileup.readString("Beams:eCM = " + std::to_string(hard.info.eCM()));
    pileup.readString("Random:setSeed = on");
    pileup.readString("Random:seed = 54321");
    if (pileupMean > 0.) check(pileup.init(), "pileup initialization failed");
    std::mt19937 random(54321);
    std::poisson_distribution<int> pileupCount(std::max(1., pileupMean));

    UltraFastSim sim;
    unsigned long long descendants = 0, recoParticles = 0, tauRecords = 0;
    unsigned long long visiblePairs = 0, acceptedPairs = 0, inputPairs = 0;
    unsigned long long visibleTausChecked = 0, oldPileupIndex15 = 0;
    using Key = std::tuple<int, int, int>; // PDG ID, mother1, ancestor tag
    for (int ev = 0; ev < nEvents;) {
      if (!hard.next()) continue;
      ++ev;
      const Pythia8::Event truth = hard.event;
      std::vector<int> expected(truth.size(), 0), tauIndices;
      std::map<int, TLorentzVector> visibleMomentum;
      std::set<int> visible, accepted, inInput;
      std::multiset<Key> expectedReco;
      for (int i = 0; i < truth.size(); ++i) {
        const auto &particle = truth[i];
        if (std::abs(particle.id()) == 15 &&
            std::abs(truth[particle.daughter1()].id()) != 15) {
          ++tauRecords;
          if (particle.pT() > 5. && std::abs(particle.eta()) < 10.) {
            tauIndices.push_back(i);
          }
        }
        if (!particle.isFinal() || !particle.isVisible()) continue;
        const int ancestor = closestTau(truth, i);
        if (ancestor) {
          expected[i] = truth[ancestor].id();
          ++descendants;
          visible.insert(expected[i]);
          visibleMomentum[ancestor] += TLorentzVector(
              particle.px(), particle.py(), particle.pz(), particle.e());
          if (selected(particle)) accepted.insert(expected[i]);
        }
        if (selected(particle)) {
          expectedReco.emplace(particle.id(), particle.mother1(), expected[i]);
        }
      }

      geawis::storeTauAncestorsInMother2(hard.event);
      for (int i = 0; i < hard.event.size(); ++i) {
        if (truth[i].isFinal() && truth[i].isVisible()) {
          check(hard.event[i].mother2() == expected[i],
                "generated descendant tag differs from original mother graph");
        }
      }
      const int nPileup = pileupMean > 0. ? pileupCount(random) : 0;
      for (int pu = 0; pu < nPileup;) {
        if (!pileup.next()) continue;
        ++pu;
        for (int i = 0; i < pileup.event.size(); ++i) {
          const auto &particle = pileup.event[i];
          if (!particle.isFinal() || !particle.isVisible() || particle.pT() <= .5)
            continue;
          if (particle.mother2() == 15) ++oldPileupIndex15;
          geawis::appendUntaggedPileup(hard.event, particle);
          if (selected(particle)) expectedReco.emplace(particle.id(), 0, 0);
        }
      }

      check(sim.run(hard.event, &hard.rndm), "UltraFastSim failed");
      std::multiset<Key> actualReco;
      const auto &particles = sim.particleList();
      for (unsigned i = 0; i < particles.size(); ++i) {
        const auto &particle = particles[i];
        actualReco.emplace(particle.GetPdgCode(), particle.GetFirstMother(),
                           particle.GetSecondMother());
        if (i > 0) check(particles[i - 1].Pt() >= particle.Pt(), "pT sort failed");
        Particle_T firmwareParticle;
        firmwareParticle.motherid = particle.GetSecondMother();
        check(firmwareParticle.motherid == particle.GetSecondMother(),
              "firmware tag conversion failed");
        if (i < NPARTICLES && std::abs(particle.GetSecondMother()) == 15) {
          inInput.insert(particle.GetSecondMother());
        }
      }
      check(actualReco == expectedReco, "smearing/sorting lost or changed tags");
      check(sim.visTauList().size() == tauIndices.size(), "visible-tau count mismatch");
      for (unsigned i = 0; i < tauIndices.size(); ++i) {
        const auto &actual = sim.visTauList()[i];
        const auto &momentum = visibleMomentum[tauIndices[i]];
        compareMomentum(actual.Px(), momentum.Px());
        compareMomentum(actual.Py(), momentum.Py());
        compareMomentum(actual.Pz(), momentum.Pz());
        compareMomentum(actual.Energy(), momentum.Energy());
        ++visibleTausChecked;
      }
      visiblePairs += visible.size();
      acceptedPairs += accepted.size();
      inputPairs += inInput.size();
      recoParticles += particles.size();
    }
    std::cout << "PASS: " << nEvents << " events, " << descendants
              << " visible tau descendants, " << recoParticles
              << " reconstructed particles; no missing or incorrect tags\n"
              << "Terminal tau records: " << tauRecords
              << "; visible-tau four-vectors checked: " << visibleTausChecked << '\n'
              << "Event/sign pairs: visible=" << visiblePairs
              << ", after pT>1 and |eta|<5=" << acceptedPairs
              << ", after top-" << NPARTICLES << "=" << inputPairs << '\n'
              << "Pileup particles whose original mother2 index was 15: "
              << oldPileupIndex15 << " (all cleared before appending)\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
