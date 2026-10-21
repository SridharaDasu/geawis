if { [ info exists env(CMSSW_BASE) ] } {
    set CMSSW_BASE $env(CMSSW_BASE)
} else {
    set CMSSW_BASE /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_15_1_2
}
set cflags "-std=c++17 -I${CMSSW_BASE}/src"

set nevents 10
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

open_project -reset algo_top_cosim

set_top algo_top

add_files -tb tb_algo_top.cpp -cflags $cflags
add_files firmware/algo_top.cpp -cflags "${cflags}"
add_files firmware/topx.cpp -cflags "${cflags}"
add_files firmware/geawis.cpp -cflags "${cflags}"

# Solution
open_solution -reset "solution"
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.778 -name default

csim_design -argv $tbArgs
csynth_design
cosim_design -argv $tbArgs

exit
