# Run this on LLVM 10 source dir

mkdir build
cd build

cmake3 -DCMAKE_INSTALL_PREFIX=$HOME/Software/llvm10 \
  -DLLVM_TARGETS_TO_BUILD="X86" \
  -DLLVM_BUILD_TOOLS=OFF \
  -DLLVM_INCLUDE_TOOLS=OFF \
  -DLLVM_BUILD_EXAMPLES=OFF \
  -DLLVM_INCLUDE_EXAMPLES=OFF \
  -DLLVM_BUILD_TESTS=OFF \
  -DLLVM_INCLUDE_TESTS=OFF \
  ..
cmake3 --build . --config Release --target install