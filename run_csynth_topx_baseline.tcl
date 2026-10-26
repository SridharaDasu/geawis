if { [ info exists env(CMSSW_BASE) ] } {
    set CMSSW_BASE $env(CMSSW_BASE)
} else {
    set CMSSW_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2
}
set cflags "-std=c++14 -I${CMSSW_BASE}/src"

# Isolated synthesis of ONLY the current/baseline get_topx (firmware/topx.cpp),
# to compare timing against the rank-based replacement (topx_rank.cpp) without
# waiting on/disturbing the full algo_top synthesis (algotop_csynth project).
open_project -reset topx_baseline_isolated
set_top get_topx
add_files firmware/topx.cpp -cflags "${cflags}"
add_files -tb tb_topx.cpp -cflags "${cflags}"

open_solution -reset "solution"
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.778 -name default

csim_design
csynth_design

exit
