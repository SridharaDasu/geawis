if { [ info exists env(CMSSW_BASE) ] } {
    set CMSSW_BASE $env(CMSSW_BASE)
} else {
    set CMSSW_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2
}
set cflags "-std=c++17 -I${CMSSW_BASE}/src"

open_project -reset geawis_stats_csim
set_top geawis_stats
add_files -tb tb_geawis.cpp -cflags "${cflags} -DNEVENTS=1000"
add_files firmware/geawis.cpp -cflags "${cflags}"

open_solution -reset solution
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.778 -name default

csim_design
exit
