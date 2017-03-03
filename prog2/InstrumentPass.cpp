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

// FunctionType *FunTy = FunctionType::get( Type::getVoidTy( MP->getContext() ), ... );
// Function *Function = dyn_cast<Function> ( MP->getOrInsertFunction(...) );
// APInt LoopId(...);
// Value *init_arg_values[] = { Constant::getIntegerValue(...), ... };
// CallInst *call = CallInst::Create(...);
// call->insertBefore(???->getFirstNonPHI());
// call->insertBefore(latch->getTerminator());

CallInst *getCallInstruction(string func_name, vector<int> values,
			LLVMContext &func_context, Module *module_pointer) {
	//initialize return and argument types
	Type *result = Type::getVoidTy(func_context);
	vector <Type *>arr;
	for (uint32_t i = 0; i < values.size(); i++) {
		arr.push_back(Type::getInt32Ty(func_context));
	}
	ArrayRef<Type *> args(arr);
	//initialize function 
	FunctionType *type = FunctionType::get(result, args, false);
	Function *init_func = dyn_cast<Function>(module_pointer->getOrInsertFunction(
				StringRef(func_name), type));
	//initialize variable instruction length size
	CallInst *call;
	if (values.size() == 1) {
		APInt arg1(32, values[0], true);
		Value *init_arg_values[1] = {Constant::getIntegerValue(Type::getInt32Ty(func_context), arg1)};
		call = CallInst::Create(init_func, init_arg_values);
	} else {
		APInt arg1(32, values[0], true);
		APInt arg2(32, values[1], true);
		Value *init_arg_values[2] = {Constant::getIntegerValue(Type::getInt32Ty(func_context), arg1),
				Constant::getIntegerValue(Type::getInt32Ty(func_context), arg2)};
		call = CallInst::Create(init_func, init_arg_values);
	}
	return call;
}

void print_DFS(map<BasicBlock *, vector<pair<BasicBlock *, int>>> *edges,
			BasicBlock *node, map<BasicBlock *, bool> *discovered,
			vector<BasicBlock *> *dfslist, int loopid, int pathid) {
	//cout << "printDFS\n" << flush;
	//node->printAsOperand(outs(), 0);
	(*discovered)[node] = true;
	dfslist->push_back(node);
	for (auto edge = (*edges)[node].begin(); edge != (*edges)[node].end(); edge++) {
		if ((*discovered)[(*edge).first] == false) {
			print_DFS(edges, (*edge).first, discovered, dfslist,
					loopid, pathid + (*edge).second);
		}
	}
	//if we are at an exit node
	if ((*edges)[node].empty()) {
		cout << "loop: " << loopid << "\t path: " << pathid
				<< "\t " << flush;
		if (dfslist->size() > 0) {
			StringRef bb_name = dfslist->front()->getName();
			if (bb_name.size() == 0) {
				cout << "no name" << flush;
				dfslist->front()->printAsOperand(outs(), 0);
			} else {
				cout << bb_name.str();
			}
		}
		for (auto path = dfslist->begin() + 1; path != dfslist->end(); path++) {
			cout << "->" << flush;
			StringRef bb_name = (*path)->getName();
			if (bb_name.size() == 0) {
				cout << "no name" << flush;
				(*path)->printAsOperand(outs(), 0);
			} else {
				cout << bb_name.str() << flush;
			}
			
			
		}
		cout << endl << flush;
	}
	(*discovered)[dfslist->back()] = false;
	dfslist->pop_back();
	return;
}

//returns true if the basic block is in the exit blocks of the loop
bool isExitBlock(Loop *loop, BasicBlock *block) {
	SmallVector<BasicBlock *, 8> exitBBs;
	loop->getExitBlocks(exitBBs);
	//block->printAsOperand(outs(), 0);
	for (auto it = exitBBs.begin(); it != exitBBs.end(); it++) {
		//(*it)->printAsOperand(outs(), 0);
		if ((*it) == block) {
			//cout << "what?" << endl;
			return true;
		}
	}
	return false;
}

bool InstrumentPass::runOnLoop(llvm::Loop* loop, llvm::LPPassManager& lpm)
{
	// MAKE SURE IT IS THE INNER MOST LOOP
	if (loop->getSubLoops().size() > 0) {
		//cout << "loop number " << loop_id << " has sub loops" << endl;
		//loop->print(outs(), 0);
		loop_id++;
		return false;
	}
	
	//LoopInfo& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
	
	//DominatorTree& domTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
	//DomTreeNode *node = domTree.getNode(loop->getLoopLatch());

	
	vector<BasicBlock *> terminators;
	terminators.push_back(loop->getLoopLatch());
	
	map<BasicBlock *, vector<BasicBlock *>> block_preds;
	//edges are a map of a block with outgoing edges in a vector with weight values on them
	map<BasicBlock *, vector<pair<BasicBlock *, int>>> edges;
	for (auto Iter = loop->block_begin(), End = loop->block_end();
			Iter != End; Iter++) {
		const TerminatorInst *TInst = (*Iter)->getTerminator();
		//(*Iter)->print(outs(), 0);
		//cout << "Successors!\n";
		for (unsigned int i = 0, NSucc = TInst->getNumSuccessors(); i < NSucc; i++) {
			BasicBlock *Succ = TInst->getSuccessor(i);
			//Succ->print(outs(), 0);
			//if the block successor is in the loop we care about it
			//if (find(loop->blocks().begin(), loop->blocks().end(), Succ) != loop->blocks().end()) {
			if (*Iter != loop->getLoopLatch()) {
				block_preds[Succ].push_back(*Iter);
				edges[*Iter].push_back(make_pair(Succ, 0));
			}
			//}
		}
	}
	
	//Kahn's toplogilical sorting algorithm
	vector<BasicBlock *> topo_order;
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
	map<BasicBlock *, int> NumPaths;
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
	//iterate reverse topological order
	for (unsigned i = topo_order.size(); i-- > 0; ) {
		BasicBlock *it = topo_order[i];
		//(*it)->printAsOperand(outs(), 0);
		CallInst *instruction;
		//if we are at the header
		if (it == loop->getHeader()) {
			instruction = getCallInstruction("init_path_reg",
					vector<int>{loop_id}, func_context, module_pointer);
			instruction->insertBefore((it)->getFirstNonPHI());
		} else if (it == loop->getLoopLatch()) {
			//if we are at the end node, insert a finalize
			instruction = getCallInstruction("finalize_path_reg",
					vector<int>{loop_id}, func_context, module_pointer);
			instruction->insertBefore((it)->getFirstNonPHI());
		} else if (isExitBlock(loop, it)) {
			//cout << "we are at the exit block?\n";
			//if we have an exit block, insert a finalize
			
			instruction = getCallInstruction("finalize_path_reg",
					vector<int>{loop_id}, func_context, module_pointer);
			instruction->insertBefore((it)->getFirstNonPHI());
			//(it)->print(outs(), 0);
		}
		
		//cout << "$$$$$$$$$$$$$$$" << isExitBlock(loop, *it) << endl;
		
		//iterate through each edge from that block
		for (auto edge = edges[it].begin(); edge != edges[it].end(); edge++) {
			//if there is one outgoing edge we put the instrumentation at the end
			//of the current basic block
			if ((edges[it].size() == 1) && ((*edge).second > 0)) {
				instruction = getCallInstruction("inc_path_reg",
						vector<int>{loop_id, (*edge).second}, func_context, module_pointer);
				instruction->insertBefore((it)->getTerminator());
			} else if ((edges[it].size() > 1) && ((*edge).second > 0)) {
				//if more than one outgoing edge then we insert at the beginning
				//of the next basicblock
				instruction = getCallInstruction("inc_path_reg",
						vector<int>{loop_id, (*edge).second}, func_context, module_pointer);
				instruction->insertBefore((*edge).first->getFirstNonPHI());
			}
		}
	}
	
	map<BasicBlock *, bool> discovered;
	vector<BasicBlock *> dfslist;
	print_DFS(&edges, topo_order.front(), &discovered, &dfslist, loop_id, 0);
	
	
	//cout << "size of the ordering is " << topo_order.size() << endl;
	//cout <<  "total number of blocks = " << loop->getNumBlocks() << endl << flush;
	
	loop_id++;
	return true;
}

void InstrumentPass::getAnalysisUsage(AnalysisUsage &AU) const
{
	//AU.addRequired<LoopInfoWrapperPass>();
	//AU.addRequired<DominatorTreeWrapperPass>();
	//AU.addRequiredTransitive<DominatorTreeWrapperPass>();
	AU.setPreservesCFG();
	return;
}

char InstrumentPass::ID = 0;
static RegisterPass<InstrumentPass> X("epp", "Efficient Path Profiling Instrument Pass.");
