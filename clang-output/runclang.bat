setlocal
set PATH=%PATH%;"c:\Software\clang6\bin"

clang -cc1 -O1 -disable-llvm-optzns -S -emit-llvm  %1
rem clang -cc1 -O2 -S -emit-llvm  %1