#ifndef EE382V_ASSIGNMENT2_EPP_H
#define EE382V_ASSIGNMENT2_EPP_H

#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"

namespace ee382v
{

class InstrumentPass: public llvm::LoopPass
{
private:
	// Private fields go here
	// int global_loop_id;

public:
	static char ID;
	InstrumentPass() : llvm::LoopPass(ID) { }

	bool runOnLoop(llvm::Loop*, llvm::LPPassManager&);

	void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
};

}

#endif
