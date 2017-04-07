//Henry Crute
//EE382V

#define DEBUG_TYPE "reach"

#include "reach.h"

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

//in[n] = union of all of the outs of the predecessors
//out[n] = gen[n] union (in[n] - kill[n])


// Print each function with something like this:
//      DataFlowAnnotator<YourAnalysisType> annotator(errs());
//      annotator.print(function);

bool reach::runOnFunction(Function &F) {
    F->dump();
    
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
static RegisterPass<reach> X("liveness", "Generic Liveness Pass.");
