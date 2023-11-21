OBJ = YMATRIX
sourcefile = main.cc YMatrix.hh YMatrix.cc LinkDict.cc Define.hh

ROOTCFLAGS  = $(shell root-config --cflags)
ROOTLIBS    = $(shell root-config --libs) -lSpectrum

CFLAGS = -g -Wall -O2 -D_LARGEFILE_SOURCE

GXX = g++ 

all:$(OBJ)

LinkDict.cc : YMatrix.hh Linkdef.h
	rootcling -f LinkDict.cc -c $^		

$(OBJ): $(sourcefile)
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) -o $@ $(filter %.cc ,$(sourcefile)) $(ROOTLIBS)
clean:
	rm -f *~ *.o $(OBJ) *Dict*
