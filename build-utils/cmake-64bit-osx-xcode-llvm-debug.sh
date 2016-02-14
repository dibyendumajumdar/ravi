mkdir xcodellvm
cd xcodellvm
cmake -DCMAKE_BUILD_TYPE=Debug -G Xcode -DLLVM_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DLLVM_DIR=$HOME/LLVM/share/llvm/cmake ..
