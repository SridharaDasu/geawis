# L1 TEST PROJECT

<p align="center">
    <img src=".structure.png" width="50%">
</p>

CMS Level-1 trigger processes data every LHC bunch crossing of 25ns. However, some systems are time multiplexed to provide additional time for processing using multiple cards. The Correlator Layer 1 (CL1) is time multiplexed at 18, much like the track trigger and the HGCAL trigger primitives systems, whereas the Correlator Layer 2 (CL2) operates time-multiplexed at 6.

- Correlator Layer 2 (CL2)

CL2 takes in puppi candidates and produces physics output. Since the CL2 input is coming from CL1, which operates time-multiplexed at 18, there are three TM18 slices processed per board in CL2.


- `algo_top` Architecture

The input to the CL2 are 32 25-Gbps links of barrel and hadron calorimeter data, of which 12 links are active for each TM6 interval. There are six more eGamma puppi candidate links that provide lower threshold objects, which are currently ignored here. The 32 links are organized in this setup as:

```
// TM18_01: B(00, 01, 02, 03, 04, 05), HE(06, 07), HFWD(08, 09), HF(30, 31)
// TM18_07: B(10, 11, 12, 13, 14, 15), HE(16, 17), HFWD(18, 09), HF(30, 31)
// TM18_13: B(20, 21, 22, 23, 24, 25), HE(26, 27), HFWD(28, 29), HF(30, 31)
```

`algo_top` gets input data every clock-tick of 360 MHz clock. So, it gets 9 64-bit candidates in one LHC bunch crossing, i.e., 9x6=54 candidates in the TM6 time interval. Total active objects received are 54x12 = 648.

The block `algo_top:processInputLinks` first selects one of above three sets of links in each TM6 interval that it processes. The other five identical cards handle (TM18_02, TM18_08, TM18_14), etc.

Within the active links for the TM6 interval the number of oobjects received are 54x12 = 648.

However, this may be too much data to process in subsequent algorithms. The second part of ``processInputLinks`` calls ``sortDescending`` to reduce the pupi candidate list down to 12x12 = 144. (Note that the ``deregionizer`` implemented by the CL2 team reduced even more to 128 candidates).

- The main algorithm playground to process puppi candidates is `geawis`

The goal is to implement a Global Event Analysis (GEA) system that digests these 144 puppi candidates using an ML-based algorithm to create a(some) score(s) to classify the event.

- Example algorithm: `StatsAlgo` Architecture

This block directly receives 144 PUPPI candidates and control token, and produces a Stats object.

## Simulation
The setup is trivial - it simply generates a dataset with random values distributed uniformly and sends it to Vitis.

### 1. Setup `geawis`
The first step is to set up geawis.
```
git clone git@github.com:SridharaDasu/geawis.git
cd geawis
```

### 2. Run csim to verify that the testbench "simulation" agrees with the code used to make the "RTL".

In this step we run csim using vitis. Compiling requires access to datatypes.h, which is in CMSSW area. If you are working on a machine with /cvmfs it should work fine. As for the access to Vitis / Vivado, you need to execute the local settings script. The example below works on Wisconsin lab machines. You could run with random input or with a file input. The data generation is in the subdirectory "data".
```
source /afs/hep.wisc.edu/cms/sw/Xilinx/Vivado/2023.1/settings64.sh
vitis_hls -f run_csim.tcl "{nevents=1000}"
vitis_hls -f run_csim.tcl "{nevents=1000, input=data/minbias.csv}"
```

### 3. Generate "RTL"

This generates the firmware RTL, which is to be used with the core framework to produce a bitfile eventually.
```
source /afs/hep.wisc.edu/cms/sw/Xilinx/Vivado/2023.1/settings64.sh
vitis_hls -f run_csynth.tcl
```
