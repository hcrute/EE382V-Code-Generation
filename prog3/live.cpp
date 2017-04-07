//Henry Crute
//EE382V

#define DEBUG_TYPE "live"

#include "live.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
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

using namespace ee382v;
using namespace llvm;
using namespace std;

//for liveness
//in[n] = use[n] union (out[n] without def[n])
//out[n] = union of all of the ins of the successors (after)


// Print each function with something like this:
//      DataFlowAnnotator<YourAnalysisType> annotator(errs());
//      annotator.print(function);

bool live::runOnFunction(Function &F) {
    F->dump();
    //create gen and kill sets for liveness
    
    //the total data domain for the given function
    
    //create analysis
    
    
    
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
static RegisterPass<live> X("liveness", "Generic Liveness Pass.");
