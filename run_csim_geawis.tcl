if { [ info exists env(CMSSW_BASE) ] } {
    set CMSSW_BASE $env(CMSSW_BASE)
} else {
    set CMSSW_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2
}
set cflags "-std=c++17 -I${CMSSW_BASE}/src"

set nevents 1000
if {[info exists env(GEAWIS_NEVENTS)]} {
    set nevents $env(GEAWIS_NEVENTS)
}
set inputFile ""
if {[info exists env(GEAWIS_INPUT)]} {
    set inputFile $env(GEAWIS_INPUT)
}
if {![regexp {^[0-9]+$} $nevents]} {
    error "GEAWIS_NEVENTS must be a non-negative integer; got '$nevents'"
}
set tbArgs [list $nevents]
if {$inputFile ne ""} {
    if {![file isfile $inputFile]} {
        error "GEAWIS_INPUT file does not exist: '$inputFile'"
    }
    lappend tbArgs [file normalize $inputFile]
}

open_project -reset geawis_stats_csim
set_top geawis_stats
add_files -tb tb_geawis.cpp -cflags $cflags
add_files firmware/geawis.cpp -cflags "${cflags}"

open_solution -reset solution
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.778 -name default

csim_design -argv $tbArgs
exit
