rmdir /s llvm9r
mkdir llvm9r
cd llvm9r
rem cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -G "Visual Studio 15 2017 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\Software\llvm900r\lib\cmake\llvm ..
cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -G "Visual Studio 16 2019" -DLLVM_JIT=ON -DLLVM_DIR=c:\Software\llvm900r\lib\cmake\llvm ..
cd ..