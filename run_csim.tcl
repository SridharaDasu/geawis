if { [ info exists env(CMSSW_BASE) ] } { 
    set CMSSW_BASE $env(CMSSW_BASE) 
} else { 
    set CMSSW_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2 
}

array set opt {
  nevents    10
}

foreach arg $::argv {
  foreach o [lsort [array names opt]] {
    regexp "$o=+(\\w+)" $arg unused opt($o)
  }
}

# Configuration
set cflags "-std=c++17 -I${CMSSW_BASE}/src"
if {$opt(nevents)} {
    append cflags " -DNEVENTS=$opt(nevents)"
}

# Project
open_project -reset "algo_top_csim"

set_top algo_top

add_files -tb tb_algo_top.cpp -cflags "${cflags}"
add_files firmware/algo_top.cpp -cflags "${cflags}"
add_files firmware/topx.cpp -cflags "${cflags}"
add_files firmware/geawis.cpp -cflags "${cflags}"

# Solution
open_solution -reset "solution"
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.778 -name default

csim_design


exit
