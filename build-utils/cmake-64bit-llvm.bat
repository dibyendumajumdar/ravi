mkdir llvm64
cd llvm64
rem pre LLVM 3.9
rem cmake -DCMAKE_INSTALL_PREFIX=c:\ravi -G "Visual Studio 14 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\LLVM37\share\llvm\cmake ..
cmake -DCMAKE_INSTALL_PREFIX=c:\ravi -G "Visual Studio 14 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\d\LLVM39_64\lib\cmake\llvm ..
cd ..