mkdir buildllvm
cd buildllvm
#cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/share/llvm/cmake ..
#cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM5/lib/cmake/llvm ..
cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/Software/ravi -DLLVM_DIR=$HOME/Software/llvm801/lib/cmake/llvm ..
