Instructions for Building With MIR JIT support
==============================================

Building with MIR support is straightforward as MIR is included in Ravi::

   mkdir buildmir
   cd buildmir
   cmake -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -DMIR_JIT=ON ..
   make install
   
That's it. 

For Windows, try this::

   mkdir buildmir
   cd buildmir
   cmake -DCMAKE_INSTALL_PREFIX=/Software/ravi -DCMAKE_BUILD_TYPE=Release -DMIR_JIT=ON ..
   cmake --build . --config Release
