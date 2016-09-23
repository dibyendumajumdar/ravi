mkdir llvm32
cd llvm32
rem cmake -DCMAKE_INSTALL_PREFIX=\d\ravi32 -G "Visual Studio 14" -DLLVM_JIT=ON -DLLVM_DIR=\d\LLVM37_32\share\llvm\cmake -DBUILD_STATIC=OFF ..
cmake -DCMAKE_INSTALL_PREFIX=\d\ravi32 -G "Visual Studio 14" -DLLVM_JIT=ON -DLLVM_DIR=\d\LLVM39\lib\cmake\llvm -DSTATIC_BUILD=OFF ..
cd ..