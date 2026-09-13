if { [ info exists env(CMSSW_BASE) ] } { 
    set CMSSW_BASE $env(CMSSW_BASE) 
} else { 
    set CMSSW_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2 
}
set cflags "-std=c++14 -I${CMSSW_BASE}/src"

# open the project
open_project -reset algotop_csynth
set_top algotop
add_files firmware/algo_top.cpp -cflags "${cflags}"
add_files firmware/topx.cpp -cflags "${cflags}"
add_files firmware/geawis.cpp -cflags "${cflags}"

# reset the solution
open_solution -reset "solution"
##   VCU118 dev kit (VU9P)
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.778 -name default

# synthethize the algorithm
csynth_design

exit
