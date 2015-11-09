setlocal
set PATH=%PATH%;"c:\Program Files (x86)\LLVM\bin"

rem clang -cc1 -O1 -disable-llvm-optzns -S -emit-llvm  %1
clang -cc1 -O2 -S -emit-llvm  %1