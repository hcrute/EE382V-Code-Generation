CXXFLAGS = $(shell llvm-config --cxxflags) -fno-rtti

all: instrument.so

InstrumentPass.o: InstrumentPass.cpp InstrumentPass.h Makefile
	$(CXX) $(CXXFLAGS) InstrumentPass.cpp -c -o $@

instrument.so: InstrumentPass.o
	$(CXX) -shared $^ -o $@

#run: instrument.so simpleloop.bc
#	opt -load ./instrument.so -disable-output -epp simpleloop.bc

#simpleloop.bc : test/simpleloop.cc
#	$(CXX) $^ -c -emit-llvm -O0 -g



clean:
	rm -f *.o *~ *.so *.ll *.bc *.out ready/*
