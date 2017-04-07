//Henry Crute
//EE382V
#ifndef EE382V_DFFramework_H
#define EE382V_DFFramework_H

#include <iostream>
#include <set>

using namespace llvm;
using namespace std;

//the state of the current basic block
class bb_state {
    //todo change
    set<int> gen;
    set<int> kill;
    set<int> in;
    set<int> out;
};

//this is binary and either union or intersect
class meet_operator {

private:
    bool mapping;
public:
    //default init values
    meet_operator(const bool map = false);
    bb_state operate(bb_state state1, bb_state state2);
};

//uses gen/kill/out/in
/*class transfer_function {
    private:
}*/

class DFAnalize {

private:
    bool direction;
    int boundary_condition;
    int initial_values;
    set<int> data_domain;
    meet_operator op;
    
public:
    //default init values
    DFAnalize(const bool dir = false);
    //set some values for analysis
    void DFSet(const bool);
    //get in and out state for a given basic block
    set<int>& getOutState(const llvm::BasicBlock& bb);
    set<int>& getInState(const llvm::BasicBlock& bb);
    
    //print the variables in DFAnalize
    void print() const;
    //start analysis
    bool start(Function &F);
    
    
};

#endif
