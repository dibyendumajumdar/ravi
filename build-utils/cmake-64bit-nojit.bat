mkdir nojit64
cd nojit64
cmake -DCMAKE_INSTALL_PREFIX=c:\ravi -DCMAKE_BUILD_TYPE=Release -DSTATIC_BUILD=ON -G "Visual Studio 15 Win64" ..
cd ..