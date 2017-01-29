#include "IRPrinter.h"
#include <iostream>
#include <typeinfo>

#include <llvm/IR/Module.h>
#include <llvm/IR/AssemblyAnnotationWriter.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FormattedStream.h>

using namespace llvm;
using namespace ee382v;
using namespace std;

bool IRPrinter::runOnModule(Module& M)
{
	// Remove the following line and before you write down your own codes
	M.print(outs(), nullptr);
	
	//function signature, including function name, return type and arguments
	
	for (auto& func: M.getFunctionList()) {
		//func.dump();
		
		//figure out of it is declared or defined
		cout << "#####declare " << flush;
		
		func.getReturnType()->print(outs(), false);
		cout << " @" << func.getName().str() << flush;
		cout << '(' << flush;
		for (auto& A: func.getArgumentList()) {
			A.print(outs(), false);
			cout << ", " << flush;
		}
		cout << ')' << flush;
		
		cout << '\n' << flush;
		
		for (auto& B: func.getBasicBlockList()) {
			cout << "; <label>:" << flush;
			B.printAsOperand(outs(), false);
			cout << ":\n" << flush;
			
			for (auto& I: B.getInstList()) {
				
				if (DebugLoc Loc = I.getDebugLoc()) {
					cout << "line " << flush;
					unsigned Line = Loc.getLine();
					cout << Line << ":" << flush;
				} else {
					cout << "line n\\a:" << flush;
				}
				
				I.print(outs());
				cout << '\n' << flush;
			}
		}
	}
	
	//not modified by the transformation
	return false;
}

void IRPrinter::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.setPreservesAll();
}

// LLVM uses the address of this static member to identify the pass, so the
// initialization value is unimportant.
char IRPrinter::ID = 0;

// Register the pass name to allow it to be called with opt:
// See http://llvm.org/releases/3.9.1/docs/WritingAnLLVMPass.html#running-a-pass-with-opt for more info.
static RegisterPass<IRPrinter> X("printIR", "IR + Source line number printer");
