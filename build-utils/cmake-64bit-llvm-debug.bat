mkdir llvm64d
cd llvm64d
cmake -DCMAKE_INSTALL_PREFIX=c:\ravi64llvmd -G "Visual Studio 14 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\LLVM37debug\share\llvm\cmake ..