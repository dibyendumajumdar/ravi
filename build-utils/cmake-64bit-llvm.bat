mkdir llvm64
cd llvm64
cmake -DCMAKE_INSTALL_PREFIX=c:\ravi -G "Visual Studio 14 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\LLVM37\share\llvm\cmake ..