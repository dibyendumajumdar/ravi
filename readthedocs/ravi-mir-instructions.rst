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

Note that the JIT compiler does not work correctly on Windows. This is due to the lack of stack unwinding
requirements of Windows.

Build Options
-------------

* `-DNO_JIT=ON` disables MIR JIT, `OFF` by default.
* `-DASAN=ON` enables ASAN, `OFF` by default.
* `-DRAVICOMP=OFF` disables the new `compiler` module, enabled by default.
* `-DLTESTS=OFF` disables internal validation tests on debug builds, enabled by default.
* `-DCMAKE_BUILD_TYPE=Debug` enables debug build.
* `-DCMAKE_BUILD_TYPE=Release` enables release build.
