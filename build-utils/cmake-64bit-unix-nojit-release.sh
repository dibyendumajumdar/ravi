mkdir buildnojit
cd buildnojit
cmake -DSTATIC_BUILD=ON -DCMAKE_BUILD_TYPE=Release -DCOMPUTED_GOTO=ON -DCMAKE_INSTALL_PREFIX=$HOME/ravi ..
