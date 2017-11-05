Assembler output from various compilers
=======================================

The clang output was generated using::

  clang -I~/github/ravi/include -std=c99 -O3 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1 -DLUA_USE_MACOSX -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk   -DRAVI_USE_COMPUTED_GOTO -S -g  -c ~/github/ravi/src/lvm.c


