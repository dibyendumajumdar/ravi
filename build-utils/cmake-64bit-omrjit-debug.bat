mkdir omrjit
cd omrjit
cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DOMR_JIT=ON ..
cd ..