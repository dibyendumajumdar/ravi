Instructions for Building With MIR JIT support
==============================================

Building with MIR support is straightforward as MIR is included in Ravi. On Linux or Mac OSX::

   mkdir build
   cd build
   cmake -DCMAKE_INSTALL_PREFIX=$HOME/Software/ravi -DCMAKE_BUILD_TYPE=Release ..
   make install
   
That's it. 

For Windows, assuming you have Visual Studio 2019 installed, you can build as follows::

   mkdir build
   cd build
   cmake -DCMAKE_INSTALL_PREFIX=/Software/ravi -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . --config Release
