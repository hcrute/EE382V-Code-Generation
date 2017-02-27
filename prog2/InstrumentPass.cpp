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
#include <utility>

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
	
	/*MDNode *loopID = loop->getLoopID();
	if (loopID == 0) {
		cout << "what the fuck" << endl;
	}*/
	
	//determine paths in loop
	
	//loopID->printAsOperand(outs(), 0);
	cout << "this is loop number " << loop_id << endl << flush;
	loop_id++;
	
	
	DominatorTree& domTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
	DomTreeNode *node = domTree.getNode(loop->getLoopLatch());

	
	/*for (auto it_node = domTree.getNode(loop->getLoopLatch()),
			  it_head = domTree.getNode(loop->getHeader());
			  it_node != it_head; ) {
		break;
	}*/
	
	map<BasicBlock *, vector<BasicBlock *>> block_preds;
	//edges are a map of a block with outgoing edges in a vector with weight values on them
	map<BasicBlock *, vector<pair<BasicBlock *, uint32_t>>> edges;
	for (auto Iter = loop->block_begin(), End = loop->block_end();
			Iter != End; Iter++) {
		const TerminatorInst *TInst = (*Iter)->getTerminator();
		//(*Iter)->print(outs(), 0);
		//cout << "Successors!\n";
		for (unsigned int i = 0, NSucc = TInst->getNumSuccessors(); i < NSucc; i++) {
			BasicBlock *Succ = TInst->getSuccessor(i);
			//Succ->print(outs(), 0);
			//if the block successor is in the loop we care about it
			if (find(loop->blocks().begin(), loop->blocks().end(), Succ) != loop->blocks().end()) {
				block_preds[Succ].push_back(*Iter);
				edges[*Iter].push_back(make_pair(Succ, 0));
			}
		}
	}
	
	
	vector<BasicBlock *> topo_order;
	
	//Kahn's toplogilical sorting algorithm
	vector<BasicBlock *> myNodes;
	myNodes.push_back(loop->getHeader());
	while (!myNodes.empty()) {
		//remove node n and add n to tail
		BasicBlock *nodeN = myNodes.back();
		myNodes.pop_back();
		topo_order.push_back(nodeN);
		//make copy of vector list
		for (auto nodeM = edges[nodeN].begin();
			      nodeM != edges[nodeN].end(); nodeM++) {
			//remove edge e from n to m in the pred
			//edges[node].back()->printAsOperand(outs(), 0);
			block_preds[(*nodeM).first].erase(remove(block_preds[(*nodeM).first].begin(),
								block_preds[(*nodeM).first].end(), nodeN), 
								block_preds[(*nodeM).first].end());

			if (block_preds[(*nodeM).first].empty()) {
				//cout << "size of this node equals zero\n";
				//node->printAsOperand(outs(), 0);
				myNodes.insert(myNodes.begin(), (*nodeM).first);
			}
		}
		
		//if we get to the loop latch we should break
		if (myNodes.back() == loop->getLoopLatch()) {
			topo_order.push_back(loop->getLoopLatch());
			break;
		}
	}
	
	//assign values to edges in DAG
	
	map<BasicBlock *, uint32_t> NumPaths;
	//loop reverse topological order
	for (unsigned i = topo_order.size(); i-- > 0; ) {
		//cout << "vertex is bb \n" << flush;
		//topo_order[i]->printAsOperand(outs(), 0);
		if (topo_order[i] == loop->getLoopLatch()) {
			NumPaths[topo_order[i]] = 1;
		} else {
			NumPaths[topo_order[i]] = 0;
			//for each edge
			for (auto edge = edges[topo_order[i]].begin();
			          edge != edges[topo_order[i]].end(); edge++) {
						  
				//(*edge).first->printAsOperand(outs(), 0);
				
				(*edge).second = NumPaths[topo_order[i]];
				//cout << "edge = " << (*edge).second << endl << flush;
				NumPaths[topo_order[i]] = NumPaths[topo_order[i]] + NumPaths[(*edge).first];
			}
		}
	}
	
	/*for (auto Iter = edges.begin(); Iter != edges.end(); Iter++) {
		cout << "block number " << flush;
		Iter->first->print(outs(), 0);
		cout << endl << flush;
		
		auto it = Iter->second.begin();
		unsigned i = 0;
		while (it != Iter->second.end()) {
			(*it).first->printAsOperand(outs(), 0);
			cout << "has a value of " << (*it).second << endl << flush;
			it++;
			i++;
		}
	}*/
	
	
	
	// FunctionType *FunTy = FunctionType::get( Type::getVoidTy( MP->getContext() ), ... );
	// Function *Function = dyn_cast<Function> ( MP->getOrInsertFunction(...) );
	// APInt LoopId(...);
	// Value *init_arg_values[] = { Constant::getIntegerValue(...), ... };
	// CallInst *call = CallInst::Create(...);
	// call->insertBefore(???->getFirstNonPHI());
	// call->insertBefore(latch->getTerminator());
	
	//LoopInfoBase< BasicBlock, Loop >::iterator loopInfo_begin;
	
	//cout << "size of the ordering is " << topo_order.size() << endl;
	//cout <<  "total number of blocks = " << loop->getNumBlocks() << endl << flush;
	
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
