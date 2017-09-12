mkdir nano
cd nano
cmake -DSTATIC_BUILD=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=c:\ravi -G "Visual Studio 15 2017 Win64" -DNANO_JIT=ON ..
cd ..