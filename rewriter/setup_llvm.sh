#!/bin/sh
mkdir llvm
cd llvm
wget http://llvm.org/releases/3.5.1/llvm-3.5.1.src.tar.xz
xz -d llvm-3.5.1.src.tar.xz
tar xf llvm-3.5.1.src.tar
rm -f llvm-3.5.1.src.tar
mv llvm-3.5.1.src llvm
cd llvm/tools
wget http://llvm.org/releases/3.5.1/cfe-3.5.1.src.tar.xz
xz -d cfe-3.5.1.src.tar.xz
tar xf cfe-3.5.1.src.tar
rm -f cfe-3.5.1.src.tar
mv cfe-3.5.1.src clang
cd clang/tools
wget http://llvm.org/releases/3.5.1/clang-tools-extra-3.5.1.src.tar.xz
xz -d clang-tools-extra-3.5.1.src.tar.xz
tar xf clang-tools-extra-3.5.1.src.tar
rm -f clang-tools-extra-3.5.1.src.tar
mv clang-tools-extra-3.5.1.src extra
cd ../../../../llvm/projects
wget http://llvm.org/releases/3.5.1/compiler-rt-3.5.1.src.tar.xz
xz -d compiler-rt-3.5.1.src.tar.xz
tar xf compiler-rt-3.5.1.src.tar
mv compiler-rt-3.5.1.src compiler-rt
cd ../..
mkdir build
cd build
../llvm/configure
make
