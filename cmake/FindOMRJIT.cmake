find_path(OMRJIT_INCLUDE_DIR nj_api.h
  PATHS
  c:/Software/ravi/include/nj
  ~/ravi/include/nj
  NO_DEFAULT_PATH
)

find_library(OMRJIT_LIBRARY
  NAMES nj libnj libnj.dylib
  PATHS
  c:/Software/ravi/lib
  ~/ravi/lib
)

set( OMRJIT_INCLUDE_DIRS "${OMRJIT_INCLUDE_DIR}" )
set( OMRJIT_LIBRARIES "${OMRJIT_LIBRARY}" )
