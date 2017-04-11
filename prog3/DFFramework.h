//Henry Crute
//EE382V
#ifndef EE382V_DFFramework_H
#define EE382V_DFFramework_H

#include <iostream>
#include <set>

using namespace llvm;
using namespace std;

void printSet(set<Value *> some);

//the state of the current basic block
class bb_state {

public:
    set<Value *> gen;
    set<Value *> kill;
    set<Value *> in;
    set<Value *> out;
};

//this is binary and either union or intersect
class meet_operator {

//private:
//    bool mapping;
public:
    //default init values
    //meet_operator(const bool map = false);
    virtual set<Value *> operate(set<Value *> state1, set<Value *> state2)
		{set<Value *> retValue; return retValue;}
    meet_operator() {};
    virtual ~meet_operator() {};
};

//uses gen/kill/out/in
class transfer_function {
	
public:
	virtual set<Value *> transfer(set<Value *> gen, set<Value *> kill,
								set<Value *> in, set<Value *> out)
		{set<Value *> retValue; return retValue;}
	transfer_function() {};
	virtual ~transfer_function() {};
};



class DFAnalize {

private:
    bool direction;
    int initial_values;
    meet_operator *meet;
    transfer_function *func;
    set<Value *> data_domain;
    set<Value *> boundary_condition;
    map<BasicBlock *, bb_state> block_states;
    
public:
    //default init values
    DFAnalize(const bool dir = false, const int ini = 0,
		meet_operator *m = new meet_operator,
		transfer_function *f = new transfer_function,
		set<Value *> domain = *(new set<Value *>),
        set<Value *> bound = *(new set<Value *>));
    //set some values for analysis
    void DFSet(const bool);
    //get in and out state for a given basic block
    set<Value *> getOutState(BasicBlock *bb);
    set<Value *> getInState(BasicBlock *bb);
    
    //set gen an kill for a block
    void setGen(BasicBlock *bb, set<Value *> somevalue);
    void setKill(BasicBlock *bb, set<Value *> somevalue);
    
    //print the variables in DFAnalize
    void print() const;
    //start analysis
    bool start(Function &F);
    
};

#endif
