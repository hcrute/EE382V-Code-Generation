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
        set_difference(out.begin(), out.end(), kill.begin(), kill.end(), 
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
    
    //create gen and kill sets for reaching definitions
    
    //the total data domain for the given function
    
    //create analysis
    
    
    
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
