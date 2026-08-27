# L1 TEST PROJECT

<p align="center">
    <img src=".structure.png" width="50%">
</p>

- `StatsAlgo` Architecture

This block directly receives 128 PUPPI candidates and control token, and produces a Stats object.

## Simulation
The setup is trivial - it simply generates a dataset with random values distributed uniformly and sends it to Vivado.

### 1. Setup `correlator-common` and `geawis`
The first step is to set up correlator-common and get access to datatypes.h from CMSSW.

```
git clone git@github.com:SridharaDasu/geawis.git
cd geawis
```

### 2. Run csim to verify that the testbench "simulation" agrees with the code used to make the "RTL".

In this step we run csim using vitis
```
source /afs/hep.wisc.edu/cms/sw/Xilinx/Vivado/2023.1/settings64.sh
vitis_hls -f run_csim.tcl "{nevents=1000}"
```

### 3. Generate "RTL"

This generates the firmware RTL, which is to be used with the core framework to produce a bitfile
```
source /afs/hep.wisc.edu/cms/sw/Xilinx/Vivado/2023.1/settings64.sh
vitis_hls -f run_csynth.tcl
```
