find_path(GCCJIT_INC libgccjit.h
  PATHS
  ~/local/include
  /usr/local/include
)

find_library(GCCJIT_LIB
  NAMES gccjit libgccjit 
  PATHS
  ~/local/lib
  /usr/local/lib/gcc/5
)

set( GCCJIT_INCLUDE_DIRS "${GCCJIT_INC}" )
set( GCCJIT_LIBRARIES "${GCCJIT_LIB}" )
