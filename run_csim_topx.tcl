if { [ info exists env(CMSSW_BASE) ] } { 
    set CMSSW_BASE $env(CMSSW_BASE) 
} else { 
    set CMSSW_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2 
}

# Configuration
set cflags "-std=c++17 -I${CMSSW_BASE}/src"

# Project
open_project -reset "topx_csim"

set_top get_topx

add_files -tb tb_topx.cpp -cflags "${cflags}"
add_files firmware/topx.cpp -cflags "${cflags}"

# Solution
open_solution -reset "solution"
set_part {xcvu13p-flga2577-2-e}
create_clock -period 3.0 -name default

csim_design

exit
