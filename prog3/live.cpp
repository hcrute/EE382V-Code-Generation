//Henry Crute
//EE382V

#define DEBUG_TYPE "live"

#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <iostream>
#include <iterator>

#include "live.h"
#include "DFFramework.h"
#include "DataFlowAnnotator.h"

using namespace ee382v;
using namespace llvm;
using namespace std;

//for liveness
//in[n] = use[n] union (out[n] without def[n])
//out[n] = union of all of the ins of the successors (after)

//meet operator for live and reaching defs
class live_meet : public meet_operator {
public:
	//returns the union of the two sets
	set<Value *> operate(set<Value *> state1, set<Value *> state2) {
		set<Value *> retValue;
		set_union(state1.begin(), state1.end(), state2.begin(), state2.end(), 
				inserter(retValue, retValue.begin()));
		return retValue;
	}
	live_meet() {};
	~live_meet() {};
};

class live_transfer : public transfer_function {
public:
	set<Value *> transfer(set<Value *> gen, set<Value *> kill,
                        set<Value *> in, set<Value *> out) {
        set<Value *> diff, retValue;
        set_difference(out.begin(), out.end(), kill.begin(), kill.end(), 
                        inserter(diff, diff.begin()));
        set_union(gen.begin(), gen.end(), diff.begin(), diff.end(), 
                        inserter(retValue, retValue.begin()));
        //cout << "difference = " << endl;
        //printSet(diff);
        //cout << "transfer = " << endl;
        //printSet(retValue);
        return retValue;
    }
	live_transfer() {};
	~live_transfer() {};
};

// Print each function with something like this:
//      DataFlowAnnotator<YourAnalysisType> annotator(errs());
//      annotator.print(function);

bool live::runOnFunction(Function &F) {
    //F.dump();
    //create analysis
    live_meet *meetop = new live_meet;
    live_transfer *transfunc = new live_transfer;
    
    set<Value *> domain;
    //initialize data domain
    for (auto& arg: F.getArgumentList()) {
		domain.emplace(&(cast<Value>(arg)));
	}
    //generate data domain and init all BB's
    for (auto& block: F.getBasicBlockList()) {
		for (auto& inst: block.getInstList()) {
			//unsigned int numOps = inst.getNumOperands();
			if (inst.getType()->getTypeID() != 0) {
				domain.emplace(&(cast<Value>(inst)));
			}
		}
	}
    
    DFAnalize liveness(true, 0, meetop, transfunc, domain);
    
    //for each basic block in the function, initialize the gen/kill
    for (auto& block: F.getBasicBlockList()) {
        set<Value *> genSet;
        set<Value *> killSet;
        for (auto& inst: block.getInstList()) {
            if (inst.getType()->getTypeID() != 0) {
				killSet.emplace(&(cast<Value>(inst)));
			}
            for (unsigned int opnum = 0; opnum < inst.getNumOperands(); opnum++) {
                Value *operand = inst.getOperand(opnum);
                //if we find operand in the set of total values
                if (domain.find(operand) != domain.end()) {
                    //if we don't find the current operand in the kill set
                    if (killSet.find(operand) == killSet.end()) {
                        genSet.emplace(operand);
                    }
                    
                }
                //cout << "operand is \n" << flush;
                //inst.getOperand(opnum)->print(outs(), false);
            }
        }
        liveness.setGen(&block, genSet);
        liveness.setKill(&block, killSet);
    }
    
    //liveness.print();
    liveness.start(F);
    
    example::DataFlowAnnotator<DFAnalize> annotator(liveness, errs());
    annotator.print(F);
    
    
    return false;
}

void live::getAnalysisUsage(AnalysisUsage &AU) const
{
	//AU.addRequired<LoopInfoWrapperPass>();
	//AU.addRequired<DominatorTreeWrapperPass>();
	//AU.addRequiredTransitive<DominatorTreeWrapperPass>();
    //AU.addRequired
	AU.setPreservesCFG();
	return;
}

char live::ID = 0;
static RegisterPass<live> X("live", "Generic Liveness Pass.");
