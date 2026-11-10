#include "firmware/geawis.h"

#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {

struct ParticleRow {
  long long event;
  unsigned particle;
  double pt;
  int eta;
  int phi;
  unsigned pid;
  double reliso;
  int motherid;
};

template <typename T>
bool parseField(const std::string &field, T &value) {
  std::istringstream input(field);
  input >> value;
  if (!input) return false;
  input >> std::ws;
  return input.eof();
}

bool parseRow(const std::string &line, ParticleRow &row) {
  std::array<std::string, 8> fields;
  std::istringstream input(line);
  for (std::size_t i = 0; i < fields.size(); ++i) {
    if (!std::getline(input, fields[i], ',')) return false;
  }
  std::string extra;
  if (std::getline(input, extra, ',')) return false;

  return parseField(fields[0], row.event) &&
         parseField(fields[1], row.particle) &&
         parseField(fields[2], row.pt) &&
         parseField(fields[3], row.eta) &&
         parseField(fields[4], row.phi) &&
         parseField(fields[5], row.pid) &&
         parseField(fields[6], row.reliso) &&
         parseField(fields[7], row.motherid) &&
         std::isfinite(row.pt) && std::isfinite(row.reliso) &&
         row.pt >= 0 && row.pt < 4096 &&
         row.eta >= -2048 && row.eta <= 2047 && row.phi >= -1024 &&
         row.phi <= 1023 && row.pid <= 7 && row.reliso >= 0 &&
         row.reliso < 1 && row.motherid >= -32768 && row.motherid <= 32767 &&
         row.particle < NPARTICLES;
}

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

bool runEvent(const std::string &fileName, long long eventNumber,
              Particle_T particles[NPARTICLES], unsigned long long &tauCount) {
  Tau_T taus[NTAUS];
  GEACtrlToken token_d;
  GEACtrlToken token_q;
  token_d.start_of_orbit = 0;
  token_d.dataValid = true;
  token_d.frameValid = true;

  geawis_taus(particles, taus, token_d, token_q);
  if (token_q.start_of_orbit != token_d.start_of_orbit ||
      token_q.dataValid != token_d.dataValid ||
      token_q.frameValid != token_d.frameValid) {
    std::cerr << fileName << ": control token was not propagated for event "
              << eventNumber << '\n';
    return false;
  }

  for (int i = 0; i < NTAUS; ++i) {
    if (!taus[i].valid || taus[i].hwPt <= 10) continue;
    const double tauPt = taus[i].hwPt.to_double();
    const double relIso = taus[i].totalIso.to_double() / tauPt;
    std::cout << fileName << " event=" << eventNumber << " pt="
              << std::fixed << std::setprecision(2) << tauPt
              << " eta=" << taus[i].hwEta.to_int()
              << " phi=" << taus[i].hwPhi.to_int()
              << " motherid=" << taus[i].motherid.to_int()
              << " charge=" << taus[i].charge.to_int()
              << " prongs=" << taus[i].nProngs.to_uint()
              << " photons=" << taus[i].nPhotons.to_uint()
              << " reliso=" << std::setprecision(4) << relIso << '\n';
    ++tauCount;
  }
  return true;
}

bool processFile(const char *fileName) {
  std::ifstream input(fileName);
  if (!input.is_open()) {
    std::cerr << "Failed to open " << fileName << '\n';
    return false;
  }

  Particle_T particles[NPARTICLES];
  clearParticles(particles);
  std::string line;
  std::size_t lineNumber = 0;
  long long currentEvent = -1;
  unsigned particlesInEvent = 0;
  unsigned long long eventCount = 0;
  unsigned long long tauCount = 0;

  while (std::getline(input, line)) {
    ++lineNumber;
    if (line.empty()) continue;
    if (lineNumber == 1 && line.find("iEvent") == 0) continue;

    ParticleRow row;
    if (!parseRow(line, row)) {
      std::cerr << fileName << ':' << lineNumber
                << ": malformed or out-of-range CSV row\n";
      return false;
    }

    if (currentEvent == -1) {
      currentEvent = row.event;
    } else if (row.event != currentEvent) {
      if (row.event <= currentEvent) {
        std::cerr << fileName << ':' << lineNumber
                  << ": event rows are not in increasing order\n";
        return false;
      }
      if (!runEvent(fileName, currentEvent, particles, tauCount)) return false;
      ++eventCount;
      clearParticles(particles);
      particlesInEvent = 0;
      currentEvent = row.event;
    }

    if (row.particle != particlesInEvent) {
      std::cerr << fileName << ':' << lineNumber
                << ": expected particle " << particlesInEvent << " in event "
                << currentEvent << ", found " << row.particle << '\n';
      return false;
    }
    particles[row.particle].hwPt = row.pt;
    particles[row.particle].hwEta = row.eta;
    particles[row.particle].hwPhi = row.phi;
    particles[row.particle].pid.bits = row.pid;
    particles[row.particle].reliso = row.reliso;
    particles[row.particle].motherid = row.motherid;
    ++particlesInEvent;
  }

  if (input.bad()) {
    std::cerr << "Failed while reading " << fileName << '\n';
    return false;
  }
  if (currentEvent == -1) {
    std::cerr << fileName << ": no event rows found\n";
    return false;
  }
  if (!runEvent(fileName, currentEvent, particles, tauCount)) return false;
  ++eventCount;
  std::cout << fileName << ": processed " << eventCount << " events, found "
            << tauCount << " taus\n";
  return true;
}

bool isStale128File(const char *fileName) {
  const std::string path(fileName);
  return path.size() >= 8 && path.compare(path.size() - 8, 8, "-128.csv") == 0;
}

}  // namespace

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " data/*.csv\n";
    return 1;
  }

  for (int i = 1; i < argc; ++i) {
    if (isStale128File(argv[i])) {
      std::cerr << "Skipping stale input file " << argv[i] << '\n';
      continue;
    }
    if (!processFile(argv[i])) return 1;
  }
  return 0;
}
