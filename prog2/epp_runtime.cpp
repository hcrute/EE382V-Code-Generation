//Henry Crute
//EE382V

#include "epp_runtime.h"

#include <iostream>
#include <map>

using namespace std;

//initialize 
//inserted into simpleloop.cc

map<int, uint32_t>regs;
map<int, map<uint32_t, uint32_t>>path_counts;

void init_path_reg(int loopId) {
	//cout << "loop id is " << loopId << endl;
	regs[loopId] = 0;
	return;
}

//increment register
//inserted into simpleloop.cc
void inc_path_reg(int loopId, int val) {
	regs[loopId] += val;
	return;
}

//add path count
//inserted into simpleloop.cc
void finalize_path_reg(int loopId) {
	path_counts[loopId][regs[loopId]]++;
	return;
}

//dump map
//called from simpleloop.cc
void dump_path_regs() {
	//path_counts[1][10] = 0;
	//path_counts[1][1] = 1;
	//loop through map
	for (auto loop = path_counts.begin(); loop != path_counts.end(); loop++) {
		for (auto path = (*loop).second.begin(); path != (*loop).second.end(); path++) {
			//cout << "path = " << (*path).first;
			cout << "loop: " << (*loop).first <<
					"\t path: " << (*path).first << "\t count: " << (*path).second << endl;
		}
	}
	return;
}

