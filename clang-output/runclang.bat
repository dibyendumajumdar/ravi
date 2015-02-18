setlocal
set PATH=%PATH%;"c:\Program Files (x86)\LLVM\bin"

clang -S -emit-llvm -O %1