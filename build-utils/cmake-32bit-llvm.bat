mkdir llvm32
cd llvm32
cmake -DCMAKE_INSTALL_PREFIX=\d\ravi32 -G "Visual Studio 14" -DLLVM_JIT=ON -DLLVM_DIR=\d\LLVM37_32\share\llvm\cmake ..
cd ..