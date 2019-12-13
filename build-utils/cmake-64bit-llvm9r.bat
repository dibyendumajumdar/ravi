rmdir /s llvm9r
mkdir llvm9r
cd llvm9r
cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -G "Visual Studio 15 2017 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\Software\llvm900r\lib\cmake\llvm ..
cd ..