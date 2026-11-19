## Tau Finder Testing Code

To run basic tau finder unit tests, use:
```
source ./setup.sh
c++ -I"$XILINX_HLS/include" -I"$CMSSW_BASE/src" -I./ tb_geawis_taus_external.cpp ../firmware/geawis.cpp -o tb_geawis_taus_external
./tb_geawis_taus
```

To run the tau finder over generated CSV events, compile and pass one or more files (the shell expands `data/*.csv`):
```
source ./setup.sh
c++ -I"$XILINX_HLS/include" -I"$CMSSW_BASE/src" -I./ tb_geawis_taus_external.cpp ../firmware/geawis.cpp -o tb_geawis_taus_external
./tb_geawis_taus_external data/minbias.csv
./tb_geawis_taus_external data/*.csv
```
The testbench reports each found tau, including its mother particle ID, and summarizes the number of events and taus per input file. It skips stale `*-128.csv` files; events with fewer than `NPARTICLES` rows are zero-padded.

The tau finder tests signal-cone radii 0.20, 0.15, and 0.10 around each
charged seed and keeps the widest valid one- or three-prong hypothesis with
unit charge. This collects wider decays while recovering compact cores when
extra tracks invalidate a larger cone. Overlapping hypotheses compete by
reconstructed pT so a signal constituent cannot appear in two output taus.
Particles outside the selected core contribute to the R=0.4 isolation cone;
the finder applies no isolation cut. The function interface is unchanged.

Use `data/hllhc-z-tau.csv` to check seed/clustering recovery before isolation.
Regenerate older CSVs with the corrected data generator first: their `motherid`
field mixes tau PDG IDs with unrelated parent indices, including index 15.
The ancestry audit and generation details are in [data/README.md](data/README.md).
For samples with `motherid = +/-15` ancestry tags, the external testbench also
reports how many event/ancestor pairs have a tagged output. It reports both
recovery without a pT cut and recovery above reconstructed pT=10 among tags
with a charged input and summed visible input pT above 10. Repeated outputs
with the same ancestor count only once. These are seed-ancestry diagnostics,
not a generated-tau efficiency: the 144-particle input can omit descendants,
and the CSV does not contain the generated tau four-vectors. Ancestry labels
are used only by the testbench, never to select or rank firmware candidates.

To make the tau histograms and eta-phi plots from its captured output, use the Python script with gnuplot:
```
source ./setup.sh
./tb_geawis_taus_external data/*.csv > tau_run.log
python3 plot_tau_output.py tau_run.log
```
The script writes two SVG dashboards per CSV (all printed taus and those with relative isolation below 10%), each with an eta-phi scatter plot and a mother particle ID histogram, plus histogram data files and the generated gnuplot script in `tau_plots/` by default. The testbench prints taus with pT above 10 and reports relative isolation as cone energy divided by reconstructed tau pT. For another simulation dataset, pass its CSV file(s) to the testbench, capture the output to a log, then pass that log to the Python script. Choose a different plot destination with `--output-dir`.
