CXXFLAGS = $(shell llvm-config --cxxflags) -fno-rtti

DEPS = DFFramework.o

all: live.so reach.so

DFFramework.o: DFFramework.cpp DFFramework.h DataFlowAnnotator.h
	$(CXX) $(CXXFLAGS) DFFramework.cpp -c -o $@

live.o: live.cpp live.h DFFramework.o
	$(CXX) $(CXXFLAGS) live.cpp -c -o $@

reach.o: reach.cpp reach.h DFFramework.o
	$(CXX) $(CXXFLAGS) reach.cpp -c -o $@ 
    
reach.so: reach.o
	$(CXX) -shared $^ $(DEPS) -o $@ 

live.so: live.o
	$(CXX) -shared $^ $(DEPS) -o $@ 

clean:
	rm -f *.o *~ *.so *.ll *.bc
