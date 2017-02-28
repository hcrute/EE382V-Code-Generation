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

//static map<

static int loop_id = 1;

CallInst *getCallInstruction(string func_name, vector<int> values,
			LLVMContext &func_context, Module *module_pointer) {
	Type *result = Type::getVoidTy(func_context);
	vector <Type *>arr;
	arr.push_back(Type::getInt32Ty(func_context));
	ArrayRef<Type *> args(arr);
	
	FunctionType *type = FunctionType::get(result, args, false);
	Function *init_func = dyn_cast<Function>(module_pointer->getOrInsertFunction(
				StringRef(func_name), type));
	APInt local_loop(32, loop_id, true);
	Value *init_arg_values[] = {Constant::getIntegerValue(
					Type::getInt32Ty(func_context), local_loop)};
	CallInst *call = CallInst::Create(init_func, init_arg_values);
	return call;
}

bool InstrumentPass::runOnLoop(llvm::Loop* loop, llvm::LPPassManager& lpm)
{
	// MAKE SURE IT IS THE INNER MOST LOOP
	LoopInfo& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
	//loop through all loops?
	for (auto it = loopInfo.begin(); it != loopInfo.end(); it++) {
		//check to see if the current loop is inside of another loop
		//return false if that is the case
		if (!LoopInfo::isNotAlreadyContainedIn(loop, *it)) {
			cout << "loop number " << loop_id << " is a parent loop";
			return false;
		}
	}
	/*if (loopInfo.getLoopDepth() != 1) {
		cout << "this loop is not at depth 1" << endl;
		return false;
	}*/
	
	cout << "this is loop number " << loop_id << endl << flush;
	
	
	DominatorTree& domTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
	DomTreeNode *node = domTree.getNode(loop->getLoopLatch());

	
	/*for (auto it_node = domTree.getNode(loop->getLoopLatch()),
			  it_head = domTree.getNode(loop->getHeader());
			  it_node != it_head; ) {
		break;
	}*/
	
	vector<BasicBlock *> terminators;
	terminators.push_back(loop->getLoopLatch());
	
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
	
	
	//terminators for exits
	//insert instrumentation into code
	Module *module_pointer = loop->getHeader()->getParent()->getParent();
	LLVMContext &func_context = loop->getHeader()->getParent()->getContext();
	
	
	
	//iterate topologically
	for (auto it = topo_order.begin(); it != topo_order.end(); it++) {
		CallInst *instruction;
		//iterate through each edge from that block
		for (auto edge = edges[*it].begin(); edge != edges[*it].end(); edge++) {
			;
		}
		//if we are at the beginning node, insert an init path reg
		if (*it == loop->getHeader()) {
			instruction = getCallInstruction("init_path_reg",
					vector<int>{0}, func_context, module_pointer);
			instruction->insertBefore((*it)->getFirstNonPHI());
		} else if (*it == loop->getLoopLatch()) {
			//if we are at the end node, insert a finalize
			instruction = getCallInstruction("finalize_path_reg",
					vector<int>{0}, func_context, module_pointer);
			instruction->insertBefore((*it)->getFirstNonPHI());;
		} else {
			//regular block
			;
		}
		
		
	}
	
	// FunctionType *FunTy = FunctionType::get( Type::getVoidTy( MP->getContext() ), ... );
	// Function *Function = dyn_cast<Function> ( MP->getOrInsertFunction(...) );
	// APInt LoopId(...);
	// Value *init_arg_values[] = { Constant::getIntegerValue(...), ... };
	// CallInst *call = CallInst::Create(...);
	// call->insertBefore(???->getFirstNonPHI());
	// call->insertBefore(latch->getTerminator());
	
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
	
	
	
	
	
	//LoopInfoBase< BasicBlock, Loop >::iterator loopInfo_begin;
	
	//cout << "size of the ordering is " << topo_order.size() << endl;
	//cout <<  "total number of blocks = " << loop->getNumBlocks() << endl << flush;
	
	//print loop info
	//loopInfo.print(outs());
	//dump this loop info
	//loop->print(outs(), 0);
	loop_id++;
	return true;
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
