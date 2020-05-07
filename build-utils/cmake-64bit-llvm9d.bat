rmdir /s llvm10d
mkdir llvm10d
cd llvm10d
cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -G "Visual Studio 16 2019" -DLLVM_JIT=ON -DLLVM_DIR=c:\Software\llvm10d\lib\cmake\llvm ..
cd ..