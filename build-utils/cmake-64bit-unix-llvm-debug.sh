mkdir buildllvmd
cd buildllvmd
#cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/share/llvm/cmake ..
#cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_JIT=ON -DLTESTS=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM5/lib/cmake/llvm ..
cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_JIT=ON -DLTESTS=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravillvm -DLLVM_DIR=$HOME/Software/llvm600/lib/cmake/llvm ..
