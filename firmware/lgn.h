#ifndef GEAWIS_LGN_H
#define GEAWIS_LGN_H

#include "data.h"

// Classifies one full 144-particle snapshot with the compiled logic gate
// network (see lgn_stuff/<model>/ for the trained model this was compiled
// from, and scripts/generate_lgn_firmware.py for how firmware/lgn.cpp was
// generated from it). lgn_class is the argmax over the class scores;
// class-index-to-label mapping is defined by the training pipeline, not by
// this firmware.
void lgn_classify(Particle_T in_particles[NPARTICLES], ap_uint<2> &lgn_class);

#endif
