rmdir /s llvm9d
mkdir llvm9d
cd llvm9d
cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -G "Visual Studio 15 2017 Win64" -DLLVM_JIT=ON -DLLVM_DIR=c:\Software\llvm900\lib\cmake\llvm ..
cd ..