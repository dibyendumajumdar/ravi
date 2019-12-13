rmdir /s llvm8
mkdir llvm8
cd llvm8
cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DLLVM_JIT=ON -DLLVM_DIR=c:\Software\llvm801\lib\cmake\llvm ..
cd ..