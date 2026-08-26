if { [ info exists env(CMSSW_RELEASE_BASE) ] } { set CMSSW_RELEASE_BASE $env(CMSSW_RELEASE_BASE) }

array set opt {
  nevents    10
}

foreach arg $::argv {
  foreach o [lsort [array names opt]] {
    regexp "$o=+(\\w+)" $arg unused opt($o)
  }
}

# Configuration
set cflags "-std=c++17 -I${CMSSW_RELEASE_BASE}/src"
if {$opt(nevents)} {
    append cflags " -DNEVENTS=$opt(nevents)"
}

# Project
open_project -reset "geawis_csim"

set_top geawis_stats

add_files -tb tb_geawis.cpp -cflags "${cflags}"
add_files -tb ./firmware/geawis.cpp -cflags "${cflags}"

# Solution
open_solution -reset "solution"
set_part {xcvu13p-flga2577-2-e}
create_clock -period 3.0 -name default

csim_design


exit
