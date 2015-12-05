mkdir buildllvmd
cd buildllvmd
cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/share/llvm/cmake ..
