#!/bin/bash
# Launch minbias/hllhc-z/ttbar data generation as three parallel, session-detached
# jobs (setsid + disown), so they survive the launching shell disconnecting.
#
# Usage: ./run_in_background.sh [numberOfEvents]
#   numberOfEvents defaults to 1500000 (~500MB/sample, based on the observed
#   ~330 bytes/event in minbias.csv at 100k events).
set -euo pipefail

cd "$(dirname "$0")"

NEVENTS="${1:-1500000}"

for c in minbias hllhc-z ttbar; do
  sed -i -E "s/^(Main:numberOfEvents[[:space:]]*=[[:space:]]*)[0-9]+/\1${NEVENTS}/" "${c}.cards"
done

for c in minbias hllhc-z ttbar; do
  setsid bash -c "
    source ../setup.sh
    echo '=== ${c} (${NEVENTS} events) ==='; date
    time ./generateData ${c}.cards
  " < /dev/null > "${c}_generateData.log" 2>&1 &
  disown
  echo "Launched ${c} (PID $!) -> ${c}_generateData.log"
done

echo "All three jobs launched in parallel. Tail progress with: tail -f data/*_generateData.log"
