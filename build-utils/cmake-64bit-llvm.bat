mkdir llvm64
cd llvm64
rem pre LLVM 3.9
rem cmake -DCMAKE_INSTALL_PREFIX=c:\ravi -G "Visual Studio 14 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\LLVM37\share\llvm\cmake ..
rem cmake -DCMAKE_INSTALL_PREFIX=c:\ravi -G "Visual Studio 15 2017 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\d\LLVM40_64\lib\cmake\llvm ..
cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DLLVM_JIT=ON -DLLVM_DIR=c:\Software\llvm501r\lib\cmake\llvm ..
cd ..
