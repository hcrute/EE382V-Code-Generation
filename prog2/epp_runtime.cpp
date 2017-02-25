//Henry Crute
//EE382V

#include "epp_runtime.h"

#include <iostream>

using namespace std;

//initialize 
//inserted into simpleloop.cc

void init_path_reg(int loopId) {
	/* 
	 * ws.add(entry);
	 * while not ws.empty() {
	 * vertex v = ws.remove();
	 * for each edge e = v->w
	 * 	if e is a chord edge
	 * 		instrument(e, 'r=Inc(e)');
	 * 	else if e is the only incoming edge of w
	 *		ws.add(w);
	 * 	else instrument(e, 'r=0'*/
	return;
}

//increment
//inserted into simpleloop.cc
void inc_path_reg(int loopId, int val) {
	return;
}

//???
//inserted into simpleloop.cc
void finalize_path_reg(int loopId) {
	return;
}

//dump current data structure
//called from simpleloop.cc
void dump_path_regs() {
	cout << "Hello World\n";
	return;
}

