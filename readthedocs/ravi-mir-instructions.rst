Instructions for Building With MIR JIT support
==============================================

Please note that currently `MIR <https://github.com/vnmakarov/mir>`_ JIT support is only available on Linux X86-64 platforms.

Building with MIR support is straightforward as MIR is included in Ravi::

   mkdir buildmir
   cd buildmir
   cmake -DCMAKE_INSTALL_PREFIX=$HOME/ravi -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -DMIR_JIT=ON ..
   make install
   
That's it. 
