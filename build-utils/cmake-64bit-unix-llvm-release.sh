mkdir buildllvm
cd buildllvm
cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/share/llvm/cmake ..
#cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM39/lib/cmake/llvm ..
