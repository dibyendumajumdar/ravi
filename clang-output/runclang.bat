setlocal
set PATH=%PATH%;"c:\Program Files (x86)\LLVM\bin"

clang -cc1 -O1 -disable-llvm-optzns -S -emit-llvm  %1