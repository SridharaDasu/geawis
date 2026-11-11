#ifndef GEAWIS_TAU_ANCESTRY_H
#define GEAWIS_TAU_ANCESTRY_H

#include "Pythia8/Event.h"

#include <cstdlib>
#include <vector>

namespace geawis {

// mother2 becomes an ancestor PDG ID, not a Pythia event index. Collect all
// descendants before changing any history, including intermediate tau copies.
// Do not run Pythia mother-history queries on the event after this conversion.
inline void storeTauAncestorsInMother2(Pythia8::Event &event) {
  std::vector<int> ancestors(event.size(), 0);
  for (int i = 0; i < event.size(); ++i) {
    if (std::abs(event[i].id()) != 15) continue;
    for (int descendant : event[i].daughterListRecursive()) {
      ancestors[descendant] = event[i].id();
    }
  }
  for (int i = 0; i < event.size(); ++i) event[i].mother2(ancestors[i]);
}

// Only final pileup particles are copied into the hard event. Their original
// parent indices belong to a different event and must not masquerade as tags
// (in particular mother2 == 15) or as hard-event mother1 indices.
inline void appendUntaggedPileup(Pythia8::Event &event,
                                Pythia8::Particle particle) {
  particle.mothers(0, 0);
  event.append(particle);
}

} // namespace geawis

#endif
