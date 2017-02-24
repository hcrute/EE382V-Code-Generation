# EE382V-Code-Gen-Optimization

for llvm makefiles use CXX=clang
for compiling down bitcode do clang hello.c -c -emit-llvm -O0 -g
for running a llvm project on it do opt -load ./prog.so -disable-output -printIR hello.bc

