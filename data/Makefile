XX           = c++
CXXFLAGS      = $(OPT2) -g -std=c++17
LD            = c++
LDFLAGS       = $(OPT2)
SOFLAGS       = -dynamiclib -single_module -undefined $(UNDEFOPT)

PYTHIA8=$(shell echo $(ROOT_INCLUDE_PATH) | sed 's/\:/\n/g' | grep pythia8 | uniq | sed 's/include//g')
$(info PYTHIA8 is $(PYTHIA8))
PYINCLUDES = -I$(PYTHIA8)/include
$(info PYINCLUDES is $(PYINCLUDES))
PYLDLIBS = -L$(PYTHIA8)/lib -lpythia8 -lpythia8lhapdf6
$(info PYLDLIBS is $(PYLDLIBS))

FASTJET=$(shell echo $(ROOT_INCLUDE_PATH) | sed 's/\:/\n/g' | grep fastjet | uniq | sed 's/include//g')
FASTJETINCLUDES = `$(FASTJET)/bin/fastjet-config --cxxflags`
$(info FASTJETINCLUDES is $(FASTJETINCLUDES))
FASTJETLDLIBS   = `$(FASTJET)/bin/fastjet-config --libs`
$(info FASTJETLDLIBS is $(FASTJETLDLIBS))

ROOTINCLUDES = -I$(ROOTSYS)/include
$(info ROOTINCLUDES is $(ROOTINCLUDES))
# Keep the C++ standard in CXXFLAGS; ROOT's --cflags would override it.
ROOTLDLIBS = $(shell $(ROOTSYS)/bin/root-config --glibs) -lEG
$(info ROOTLDLIBS is $(ROOTLDLIBS))

XILINXINCLUDES=-I$(XILINX_HLS)/include

CMSSWINCLUDES=-I${CMSSW_BASE}/src

INCLUDES = -I. -I../.. $(PYINCLUDES) $(FASTJETINCLUDES) $(ROOTINCLUDES) $(XILINXINCLUDES) $(CMSSWINCLUDES)
LDLIBS = -L. $(PYLDLIBS) $(FASTJETLDLIBS) $(ROOTLDLIBS)

all : generateData
	@echo Built $@

clean :
	rm -rf generateData tb_tau_ancestry *.a *.o

generateData : generateData.cc libGEAWisData.a TauAncestry.h
	$(LD) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(filter %.cc %.a,$^) $(LDLIBS) -o $@
	@echo Built $@

tb_tau_ancestry : tb_tau_ancestry.cpp libGEAWisData.a TauAncestry.h
	$(LD) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(filter %.cpp %.a,$^) $(LDLIBS) -o $@

.PHONY: test-tau-ancestry
test-tau-ancestry : tb_tau_ancestry
	./tb_tau_ancestry hllhc-z-tau.cards 1000 25

libGEAWisData.a : libGEAWisData.a(EventAnalysis.o) libGEAWisData.a(UltraFastSim.o)
	@echo Built $@

EventAnalysis.cc : EventAnalysis.h

UltraFastSim.cc : UltraFastSim.h EventData.h

.cc.a :
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $^
	ar rv $@ $*.o
	rm -f $*.o
	@echo Compiled $@
