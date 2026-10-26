if { [ info exists env(CMSSW_BASE) ] } {
    set CMSSW_BASE $env(CMSSW_BASE)
} else {
    set CMSSW_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2
}
set cflags "-std=c++14 -I${CMSSW_BASE}/src"

# Isolated synthesis of the rank-based replacement (firmware/topx_rank.cpp),
# to compare timing against the current/baseline get_topx (see
# run_csynth_topx_baseline.tcl) without waiting on/disturbing the full
# algo_top synthesis (algotop_csynth project).
open_project -reset topx_rank_isolated
set_top get_topx_rank
add_files firmware/topx_rank.cpp -cflags "${cflags}"
add_files -tb tb_topx_rank.cpp -cflags "${cflags}"

open_solution -reset "solution"
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.778 -name default

csim_design
csynth_design

exit
