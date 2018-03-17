mkdir nojit64a
cd nojit64a
cmake -DCMAKE_INSTALL_PREFIX=c:\Software\ravi -DCMAKE_BUILD_TYPE=Debug -DLTESTS=ON -G "Visual Studio 15 2017 Win64" ..
cd ..