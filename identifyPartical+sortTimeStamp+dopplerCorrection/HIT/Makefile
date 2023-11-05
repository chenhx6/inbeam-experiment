OBJ = Hit
sourcefile = main.cc YHit.hh YHit.cc Define.hh LinkDict.cc

ROOTCFLAGS  = $(shell root-config --cflags)
ROOTLIBS    = $(shell root-config --libs) -lSpectrum

CFLAGS = -g -Wall -O2 

GXX = g++ 

all:$(OBJ)

LinkDict.cc : YHit.hh Linkdef.h
	rootcling -f LinkDict.cc -c $^		

$(OBJ): $(sourcefile)
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) -o $@ $(filter %.cc ,$(sourcefile)) $(ROOTLIBS)
clean:
	rm -f *~ *.o $(OBJ) *Dict*
