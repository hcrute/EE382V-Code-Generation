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
    //create gen and kill sets for liveness
    
    //the total data domain for the given function
    
    //create analysis
    DFAnalize liveness(true, 0);
    
    //for each basic block in the function, initialize the gen/kill
    for (auto& block: F.getBasicBlockList()) {
        set<Value *> genSet;
        set<Value *> killSet;
        for (auto& inst: block.getInstList()) {
            if (inst.getType()->getTypeID() != 0) {
				killSet.emplace(&(cast<Value>(inst)));
                
                //cout << "instruction:" << endl;
                //inst.print(outs(), false);
                for (auto use = inst.user_begin(); use != inst.user_end(); use++) {
                    //cout << "\nadding this to kill set\n" << flush;
                    //(*use)->print(outs(), false);
                    genSet.emplace((cast<Value>(*use)));
                }
			}
            
        }
        liveness.setGen(&block, genSet);
        liveness.setKill(&block, killSet);
    }
    
    //liveness.print();
    liveness.start(F);
    
    
    return false;
}

void live::getAnalysisUsage(AnalysisUsage &AU) const
{
	//AU.addRequired<LoopInfoWrapperPass>();
	//AU.addRequired<DominatorTreeWrapperPass>();
	//AU.addRequiredTransitive<DominatorTreeWrapperPass>();
	AU.setPreservesCFG();
	return;
}

char live::ID = 0;
static RegisterPass<live> X("live", "Generic Liveness Pass.");
