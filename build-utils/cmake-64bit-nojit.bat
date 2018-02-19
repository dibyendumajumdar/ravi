mkdir nojit64
cd nojit64
cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -DCMAKE_BUILD_TYPE=Debug -DSTATIC_BUILD=ON -G "Visual Studio 15 2017 Win64" ..
cd ..