mkdir nanorelease
cd nanorelease
cmake -DSTATIC_BUILD=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=c:\ravi -G "Visual Studio 15 2017 Win64" -DNANO_JIT=ON ..
cd ..