find_path(OMRJIT_INCLUDE_DIR nj_api.h
  PATHS
  c:/Software/omr/include/nj
  ~/Software/omr/include/nj
  NO_DEFAULT_PATH
)

find_library(OMRJIT_LIBRARY
  #NAMES nj libnj libnj.dylib
  NAMES jitbuilder libjitbuilder libjitbuilder.dylib
  PATHS
  c:/Software/omr/lib
  ~/Software/omr/lib
)

set( OMRJIT_INCLUDE_DIRS "${OMRJIT_INCLUDE_DIR}" )
set( OMRJIT_LIBRARIES "${OMRJIT_LIBRARY}" )
