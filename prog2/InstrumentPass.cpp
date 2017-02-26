#define DEBUG_TYPE "InstrumentPass"

#include "InstrumentPass.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>

#include <iostream>

using namespace ee382v;
using namespace llvm;
using namespace std;


static map<uint64_t, map<uint64_t, uint64_t>> path_counts;

//static map<

static uint64_t loop_id = 1;

bool InstrumentPass::runOnLoop(llvm::Loop* loop, llvm::LPPassManager& lpm)
{
	// Write your CODE Here
	// I intentionally kept some code here so that you can know some APIs to call
	// Delete them if you find them annoying :)
	
	
	// MAKE SURE IT IS THE INNER MOST LOOP?
	LoopInfo& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
	/*if (loopInfo.getLoopDepth() != 1) {
		cout << "this loop is not at depth 1" << endl;
		return false;
	}*/
	
	
	
	// FunctionType *FunTy = FunctionType::get( Type::getVoidTy( MP->getContext() ), ... );
	// Function *Function = dyn_cast<Function> ( MP->getOrInsertFunction(...) );
	// APInt LoopId(...);
	// Value *init_arg_values[] = { Constant::getIntegerValue(...), ... };
	// CallInst *call = CallInst::Create(...);
	// call->insertBefore(???->getFirstNonPHI());
	// call->insertBefore(latch->getTerminator());
	
	/*MDNode *loopID = loop->getLoopID();
	if (loopID == 0) {
		cout << "what the fuck" << endl;
	}*/
	
	//determine paths in loop
	
	//loopID->printAsOperand(outs(), 0);
	cout << "this is loop number " << loop_id << endl;
	loop_id++;
	
	/*LoopBase< BasicBlock, Loop>::iterator loop_begin;
	
	for (loop_begin = loop->begin();
			loop_begin != loop->end();
			loop_begin++) {
		cout << "okay!!!!!!!!!!!!!!!!..." << endl;
	}*/
	
	DominatorTree& domTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
	//gets the loop header as a dominator tree?
	DomTreeNode *node = domTree.getNode(loop->getHeader());
	
	//how many paths there are for a certain vertex
	map<uint64_t, uint64_t> num_paths;
	uint32_t i = 0;
	//entry node is loop header
	//exit node is loop latch
	
	//assign values to edges in DAG
	for (auto Iter = loop->block_begin(), End = loop->block_end();
			Iter != End; Iter++, i++) {
		//(*Iter)->print(outs(), 0);
		//if v is a leaf vertex
		if (*Iter == loop->getLoopLatch()) {
			cout << "we are at the latch\n";
			num_paths[i] = 1;
		} else {
			cout << "not latch\n";
			num_paths[i] = 0;
			(*Iter)->print(outs(), 0);
		}
		
	}
	
	
	//loopHeader->print(outs(), 0);
	
	LoopInfoBase< BasicBlock, Loop >::iterator loopInfo_begin;
	
	/*for (loopInfo_begin = loopInfo.begin();
			loopInfo_begin != loopInfo.end();
			loopInfo_begin++) {
		cout << "alright........ " << endl;		
	}*/
	
	//cout <<  "please print" << loop->getNumBlocks() << endl;
	
	//print loop info
	//loopInfo.print(outs());
	//dump this loop info
	//loop->print(outs(), 0);
	
	return false;
}

void InstrumentPass::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.addRequired<LoopInfoWrapperPass>();
	AU.addRequired<DominatorTreeWrapperPass>();
	AU.addRequiredTransitive<DominatorTreeWrapperPass>();
	AU.setPreservesCFG();
	return;
}

char InstrumentPass::ID = 0;
static RegisterPass<InstrumentPass> X("epp", "Efficient Path Profiling Instrument Pass.");
