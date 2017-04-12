//Henry Crute
//EE382V

#define DEBUG_TYPE "reach"


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


#include "reach.h"
#include "DFFramework.h"
#include "DataFlowAnnotator.h"

using namespace ee382v;
using namespace llvm;
using namespace std;

//in[n] = union of all of the outs of the predecessors
//out[n] = gen[n] union (in[n] - kill[n])


class reach_meet : public meet_operator {
public:
	set<Value *> operate(set<Value *> state1, set<Value *> state2) {
		set<Value *> retValue;
		set_union(state1.begin(), state1.end(), state2.begin(), state2.end(), 
				inserter(retValue, retValue.begin()));
		return retValue;
	}
	reach_meet() {};
	~reach_meet() {};
};

//transfer functions for live and reaching defs

class reach_transfer : public transfer_function {
public:
	set<Value *> transfer(set<Value *> gen, set<Value *> kill,
                set<Value *> in, set<Value *> out) {
        set<Value *> diff, retValue;
        set_difference(in.begin(), in.end(), kill.begin(), kill.end(), 
                        inserter(diff, diff.begin()));
        set_union(gen.begin(), gen.end(), diff.begin(), diff.end(), 
                        inserter(retValue, retValue.begin()));
        return retValue;
    }
	reach_transfer() {};
	~reach_transfer() {};
};

// Print each function with something like this:
//      DataFlowAnnotator<YourAnalysisType> annotator(errs());
//      annotator.print(function);

bool reach::runOnFunction(Function &F) {
    //F.dump();
    //create analysis
    reach_meet *meetop = new reach_meet;
    reach_transfer *transfunc = new reach_transfer;
    
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
    
    DFAnalize reaching(false, 0, meetop, transfunc, domain);
    
    //for each basic block in the function, initialize the gen/kill
    for (auto& block: F.getBasicBlockList()) {
        set<Value *> genSet;
        set<Value *> killSet;
        for (auto& inst: block.getInstList()) {
            if (inst.getType()->getTypeID() != 0) {
				killSet.emplace(&(cast<Value>(inst)));
                genSet.emplace(&(cast<Value>(inst)));
			}
        }
        
        reaching.setGen(&block, genSet);
        reaching.setKill(&block, killSet);
    }
    
    reaching.print();
    reaching.start(F);
    
    example::DataFlowAnnotator<DFAnalize> annotator(reaching, errs());
    //annotator.print(F);
    
    return false;
}

void reach::getAnalysisUsage(AnalysisUsage &AU) const
{
	//AU.addRequired<LoopInfoWrapperPass>();
	//AU.addRequired<DominatorTreeWrapperPass>();
	//AU.addRequiredTransitive<DominatorTreeWrapperPass>();
	AU.setPreservesCFG();
	return;
}

char reach::ID = 0;
static RegisterPass<reach> X("reach", "Generic Reaching Definition Pass.");
