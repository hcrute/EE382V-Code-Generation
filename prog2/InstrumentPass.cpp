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
#include <algorithm>

#include <iostream>

using namespace ee382v;
using namespace llvm;
using namespace std;


static map<uint64_t, map<uint64_t, uint64_t>> path_counts;

//static map<

static uint64_t loop_id = 1;

void visit() {
	return;
}

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
	cout << "this is loop number " << loop_id << endl << flush;
	loop_id++;
	
	/*LoopBase< BasicBlock, Loop>::iterator loop_begin;
	
	for (loop_begin = loop->begin();
			loop_begin != loop->end();
			loop_begin++) {
		cout << "okay!!!!!!!!!!!!!!!!..." << endl;
	}*/
	
	DominatorTree& domTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
	DomTreeNode *node = domTree.getNode(loop->getLoopLatch());
	//node->getBlock()->print(outs(), 0);
	
	//how many paths there are for a certain vertex
	
	uint32_t i = 0;
	//entry node is loop header
	//exit node is loop latch
	
	
	
	for (auto it_node = domTree.getNode(loop->getLoopLatch()),
			  it_head = domTree.getNode(loop->getHeader());
			  it_node != it_head; ) {
		break;
	}
	
	map<BasicBlock *, vector<BasicBlock *>> block_preds;
	map<BasicBlock *, vector<BasicBlock *>> edges;
	//assign values to edges in DAG
	for (auto Iter = loop->block_begin(), End = loop->block_end();
			Iter != End; Iter++, i++) {
		const TerminatorInst *TInst = (*Iter)->getTerminator();
		//(*Iter)->print(outs(), 0);
		//cout << "Successors!\n";
		for (unsigned i = 0, NSucc = TInst->getNumSuccessors(); i < NSucc; i++) {
			BasicBlock *Succ = TInst->getSuccessor(i);
			//Succ->print(outs(), 0);
			block_preds[Succ].push_back(*Iter);
			edges[*Iter].push_back(Succ);
		}
	}
	
	
	vector<BasicBlock *> reverse_top;
	vector<BasicBlock *> visited_blocks;
	
	BasicBlock *prevNode = NULL;
	unsigned total_preds = 0;
	unsigned curr_preds = 0;
	//Kahn's algorithm
	vector<BasicBlock *> myNodes;
	myNodes.push_back(loop->getHeader());
	while (myNodes.size() > 0) {
		BasicBlock *node = myNodes.back();
		myNodes.pop_back();
		reverse_top.push_back(node);
		
		
		node->printAsOperand(outs(), 0);
		
		const TerminatorInst *TInst = reverse_top.back()->getTerminator();
		for (unsigned i = 0, NSucc = TInst->getNumSuccessors(); i < NSucc; i++) {
			BasicBlock *Succ = TInst->getSuccessor(i);
			if (find(loop->blocks().begin(), loop->blocks().end(), Succ) != loop->blocks().end()) {
				edges[node].erase(find(edges[node].begin(),
								   edges[node].end(), Succ), 
								   edges[node].end());
				if (edges[node].size() == 0) {
					myNodes.insert(myNodes.begin(), Succ);
				}
			} else {
				cout << "couldn't find\n";
			}
		}
		
		if (myNodes.back() == loop->getLoopLatch()) {
			reverse_top.push_back(loop->getLoopLatch());
			break;
		}
	}
	
	for (auto Iter = reverse_top.begin(); Iter != reverse_top.end(); Iter++) {
		(*Iter)->printAsOperand(outs(), 0);
	}
	cout << "size of the ordering is " << reverse_top.size() << endl;
	
	
	
	/*for (auto Iter = block_preds.begin(); Iter != block_preds.end(); Iter++) {
		cout << "block number " << flush;
		Iter->first->printAsOperand(outs(), 0);
		cout << endl << flush;
		
		auto it = Iter->second.begin();
		unsigned i = 0;
		while (it != Iter->second.end()) {
			(*it)->print(outs(), 0);
			it++;
			i++;
		}
	}*/
	
	
	//loop->print(outs(), 0);
	
	LoopInfoBase< BasicBlock, Loop >::iterator loopInfo_begin;
	
	/*for (loopInfo_begin = loopInfo.begin();
			loopInfo_begin != loopInfo.end();
			loopInfo_begin++) {
		cout << "alright........ " << endl;		
	}*/
	
	cout <<  "total number of blocks = " << loop->getNumBlocks() << endl << flush;
	
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
