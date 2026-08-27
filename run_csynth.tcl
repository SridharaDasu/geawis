if { [ info exists env(CMSSW_RELEASE_BASE) ] } { 
    set CMSSW_RELEASE_BASE $env(CMSSW_RELEASE_BASE) 
} else { 
    set CMSSW_RELEASE_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2 
}
set cflags "-std=c++17 -I${CMSSW_RELEASE_BASE}/src"

# open the project
open_project -reset geawis_csynth
set_top geawis_stats
add_files firmware/geawis.cpp -cflags "${cflags}"

# reset the solution
open_solution -reset "solution"
##   VCU118 dev kit (VU9P)
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.3
# set_clock_uncertainty 0.2

# synthethize the algorithm
csynth_design

exit
