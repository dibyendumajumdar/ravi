mkdir llvm64d
cd llvm64d
rem cmake -DCMAKE_INSTALL_PREFIX=c:\ravi64llvmd -G "Visual Studio 14 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\LLVM37debug\share\llvm\cmake ..
cmake -DSTATIC_BUILD=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=c:\d\ravi64llvmd -G "Visual Studio 15 2017 Win64" -DLLVM_JIT=ON -DEMBEDDED_C_JIT=ON -DLLVM_DIR=c:\d\LLVM39D64\lib\cmake\llvm ..
cd ..