//Henry Crute
//EE382V

//providing the following information
//we can use this framework to develop a data flow analysis pass

//domain (the data structure to represent data-flow values for 
//analysis it is the set of variables or a bit vector of variables)

//direction

//transfer functions

//meet operator

//initial flow values

//boundary condition (flow value for entry or exit)

//llvm includes
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

//standard library includes
#include <iostream>
#include <map>
#include <set>

//my file includes
#include "DFFramework.h"
#include "DataFlowAnnotator.h"

using namespace std;
using namespace llvm;

//implementation of meet_operator

meet_operator::meet_operator(const bool map) :
mapping(map)
{}

//if mappingn is true then union
//if mapping is false then intersect
bb_state meet_operator::operate(bb_state state1, bb_state state2) {
	bb_state result;
	return result;
};

//implementation of DFAnalize
DFAnalize::DFAnalize(const bool dir, const int ini) :
direction(dir), initial_values(ini)
{}

//print the current values in dfanalize
void DFAnalize::print() const {
    if (direction) {
        cout << "direction is forward" << endl << flush;
    } else {
        cout << "direction is backward" << endl << flush;
    }
    if (initial_values) {
        cout << "initial values are 1 for the global set" << endl << flush;
    } else {
        cout << "intial values are 0 for the global set" << endl << flush;
    }
    
}

//set some values for the analysis
void DFAnalize::DFSet(const bool dir) {
    direction = dir;
}

/*bb_state& DFAnalize::getInState(const llvm::BasicBlock& bb) {
    ;
}

bb_state& DFAnalize::getOutState(const llvm::BasicBlock& bb) {
    ;
}*/

//start analysis until we converge
bool DFAnalize::start(Function &F) {
    //use the initialization value of the set to initialize in/out to everything or nothing
    
    //mapping of basic blocks with their state
    //map<BasicBlock&, bb_state&> block_states;
    
    //generate data domain and init all BB's
    for (auto& block: F.getBasicBlockList()) {
		cout << "some shit \n";
		block.printAsOperand(outs(), false);
	}
    
    
    
    //init all BB's
    if (initial_values > 0) {
        
    } else {
        
    }
    
    
    if (direction) {
        cout << "direction is forward" << endl << flush;
    } else {
        cout << "direction is backward" << endl << flush;
    }
    return true;
}

