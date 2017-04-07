#! /bin/sh

rm -f *.ll *.bc ./input ./output
make clean
clang -emit-llvm -c sample_input/$1.c
cp $1.bc sample_input
opt -instnamer $1.bc | opt -mem2reg -o input.bc
llvm-dis input.bc -o input.ll
make CXX=clang
echo ----Live----
#opt -load ./live.so -live input.bc -disable-output > sample_input/$1.live 2>&1
opt -load ./live.so -live input.bc -disable-output 2>&1 | tee sample_input/$1.live
echo ----Reach----
opt -load ./reach.so -reach input.bc -disable-output 2>&1 | tee sample_input/$1.reach

