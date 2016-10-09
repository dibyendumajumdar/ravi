mkdir llvm32d
cd llvm32d
rem cmake -DCMAKE_INSTALL_PREFIX=\d\ravi32 -G "Visual Studio 14" -DLLVM_JIT=ON -DLLVM_DIR=\d\LLVM37_32\share\llvm\cmake ..
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=\d\ravi32 -G "Visual Studio 14" -DLLVM_JIT=ON -DLLVM_DIR=\d\LLVM39D_32\lib\cmake\llvm -DSTATIC_BUILD=ON ..
cd ..