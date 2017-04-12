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
#include "llvm/IR/CFG.h"

//standard library includes
#include <iostream>
#include <map>
#include <set>
#include <typeinfo>

//my file includes
#include "DFFramework.h"
#include "DataFlowAnnotator.h"

using namespace std;
using namespace llvm;

void printSet(set<Value *> some) {
	cout << "print set start \n" << flush;
	for (auto something : some) {
		something->print(outs(), false);
		cout << "type is " << something->getType()->getTypeID() << flush;
		cout << endl << flush;
	}
	return;
}

//implementation of DFAnalize
DFAnalize::DFAnalize(const bool dir, const int ini,
	meet_operator *m, transfer_function *f, set<Value *> d, set<Value *> b) :
direction(dir), initial_values(ini),
meet(m), func(f), data_domain(d), boundary_condition(b) {
	
}


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
    for (auto myblock = block_states.begin(); myblock != block_states.end(); myblock++) {
		cout << "THIS IS THE BLOCK" << endl << flush;
		myblock->first->print(outs(), false);
		
		cout << "THIS IS THE GEN" << endl << flush;
		for (auto mygen = myblock->second.gen.begin(); mygen != myblock->second.gen.end(); mygen++) {
			(*mygen)->print(outs(), false);
			cout << endl << flush;
		}
		
		cout << "THIS IS THE KILL" << endl << flush;
		for (auto mykill = myblock->second.kill.begin(); mykill != myblock->second.kill.end(); mykill++) {
			(*mykill)->print(outs(), false);
			cout << endl << flush;
		}
	}
    
}

void DFAnalize::setGen(BasicBlock *bb, set<Value *> somevalue) {
	block_states[bb].gen = somevalue;
	return;
}

void DFAnalize::setKill(BasicBlock *bb, set<Value *> somevalue) {
	block_states[bb].kill = somevalue;
	return;
}

//set some values for the analysis
void DFAnalize::DFSet(const bool dir) {
    direction = dir;
}

//start analysis until we converge
//operands can be value constants, blocks, or instructions

//only track liveness of instruction-defined values and
//function arguments


set<Value *> DFAnalize::getInState(BasicBlock *bb) {
	return block_states[bb].in;
}

set<Value *> DFAnalize::getOutState(BasicBlock *bb) {
	return block_states[bb].out;
}


bool DFAnalize::start(Function &F) {
    //use the initialization value of the set to initialize in/out to everything or nothing
    
    //mapping of basic blocks with their state
    //map<BasicBlock&, bb_state&> block_states;
	
    
    //printSet(data_domain);
    
    //if we need to init the values to entire data_domain
    if (initial_values > 0) {
		for (auto& block: F.getBasicBlockList()) {
			if (direction) {
				//init out
				block_states[&block].out.insert(data_domain.begin(), data_domain.end());
			} else {
				//init in
				block_states[&block].in.insert(data_domain.begin(), data_domain.end());
			}
		}
    }
    
    //initialize the boundary condition for forward or backward analysis
    //backward so initialize in of basic block w/o a predecessor (entry)
    if (direction == true) {
        for (auto& block: F.getBasicBlockList()) {
            if (pred_begin(&block) == pred_end(&block)) {
                cout << "boundary condition set (backward)\n";
                block_states[&block].in = boundary_condition;
            }
        }
    //forward so initialize out of basic block w/o a successor (exit)
    } else {
        for (auto& block: F.getBasicBlockList()) {
            if (succ_begin(&block) == succ_end(&block)) {
                cout << "boundary condition set (forward)\n";
                block_states[&block].out = boundary_condition;
            }
        }
    }
    
    bool changed = true;
    //loop over until we converge
    while (changed) {
		//cout << "changed is " << changed << endl;
		changed = false;
		//if backward then we compute in with function and then out with meet
		if (direction == true) {
			//compute in
			for (auto& block: F.getBasicBlockList()) {
				//gen, kill, in, out
                /*IF STATEMENT THAT USES BOUNDARY CONDITION */
				if (pred_begin(&block) == pred_end(&block)) {
                    continue;
                }
				
				bb_state *mystate = &block_states[&block];
				set<Value *> newIn = func->transfer(mystate->gen, mystate->kill,
					mystate->in, mystate->out);
				//printSet(newIn);
				//check to see if we changed the in set
				if (newIn != mystate->in) {
					changed = true;
					//cout << "before in = ";
					//printSet(mystate->in);
					//cout << "after in = ";
					//printSet(newIn);
					mystate->in = newIn;
				}
			}
			
			//compute out for each bb based on in
			for (auto& block: F.getBasicBlockList()) {
				//block.print(outs(), false);
				//cout << endl << flush;
				
				//get list of successors
				vector<BasicBlock *> after;
				for (BasicBlock *succ : successors(&block)) {
					after.push_back(succ);
				}
				
				set<Value *> newOut;
				if (after.size() == 0) {
					//cout << "size equals 0" << endl;
				} else if (after.size() == 1) {
					//cout << "size equals 1" << endl;
					newOut = block_states[after.back()].in;
				} else {
					//cout << "size equals " << after.size() << endl;
					newOut = block_states[after.back()].in;
					after.pop_back();
					while (after.size() != 0) {
						newOut = meet->operate(newOut, block_states[after.back()].in);
						after.pop_back();
						//cout << "did loop \n" << flush;
					}
				}
				if (newOut != block_states[&block].out) {
					changed = true;
					block_states[&block].out = newOut;
				}
			}
		//forward analysis
		} else {
			//compute out
			for (auto& block: F.getBasicBlockList()) {
				
				/*IF STATEMENT THAT USES BOUNDARY CONDITION */
                if (succ_begin(&block) == succ_end(&block)) {
                    continue;
                }
                
				bb_state *mystate = &(block_states[&block]);
				set<Value *> newOut = func->transfer(mystate->gen, mystate->kill,
					mystate->in, mystate->out);
				if (newOut != mystate->out) {
					changed = true;
					//cout << "before in = ";
					//printSet(mystate->in);
					//cout << "after in = ";
					//printSet(newIn);
					mystate->out = newOut;
				}
			}
			
			//compute in for each bb based on out
			for (auto& block: F.getBasicBlockList()) {
				//list of predecessors
				vector<BasicBlock *> before;
				for (BasicBlock *pred : predecessors(&block)) {
					before.push_back(pred);
				}
				
				set<Value *> newIn;
				if (before.size() == 0) {
					//cout << "size equals 0" << endl;
				} else if (before.size() == 1) {
					//cout << "size equals 1" << endl;
					newIn = block_states[before.back()].out;
				} else {
					//cout << "size equals " << after.size() << endl;
					newIn = block_states[before.back()].out;
					before.pop_back();
					while (before.size() != 0) {
						newIn = meet->operate(newIn, block_states[before.back()].out);
						before.pop_back();
						//cout << "did loop \n" << flush;
					}
				}
				if (newIn != block_states[&block].in) {
					changed = true;
					
					block_states[&block].in = newIn;
				}
			}
		}
	}
    
    return true;
}

