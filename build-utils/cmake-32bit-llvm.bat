mkdir llvm32
cd llvm32
cmake -DCMAKE_INSTALL_PREFIX=c:\ravi32llvm -G "Visual Studio 14" -DLLVM_JIT=ON -DLLVM_DIR=c:\LLVM37_32\share\llvm\cmake ..