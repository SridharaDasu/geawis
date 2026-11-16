if { [ info exists env(CMSSW_BASE) ] } {
    set CMSSW_BASE $env(CMSSW_BASE)
} else {
    set CMSSW_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2
}

# Configuration
set cflags "-std=c++17 -I${CMSSW_BASE}/src"

# Project
open_project -reset "lgn_csim"

set_top lgn_circuit

add_files -tb tb_lgn.cpp -cflags "${cflags}"
add_files -tb lgn_stuff/LogicModelTiny_20261102-180844/circuit.c
add_files firmware/lgn.cpp -cflags "${cflags}"

# Solution
open_solution -reset "solution"
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.778 -name default

csim_design

exit
