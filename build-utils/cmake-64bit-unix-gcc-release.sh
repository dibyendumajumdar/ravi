mkdir buildgcc
cd buildgcc
cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_JIT=OFF -DGCC_JIT=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi ..
