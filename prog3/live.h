//Henry Crute
//EE382V

#ifndef EE382V_LIVE_H
#define EE382V_LIVE_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

namespace ee382v
{

class live: public llvm::FunctionPass
{
private:
	// Private fields go here
	// int global_loop_id;

public:
	static char ID;
	live() : llvm::FunctionPass(ID) { }

	bool runOnFunction(llvm::Function &F);

	void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
};

}

#endif
