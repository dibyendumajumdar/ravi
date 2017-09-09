find_path(NANOJITEXTRA_INCLUDE_DIR nanojitextra.h
  PATHS
  c:/ravi/include/nanojit
  ~/ravi/include/nanojit
  NO_DEFAULT_PATH
)

find_library(NANOJITEXTRA_LIBRARY
  NAMES nanojitextra libnanojitextra libnanojitextra.dylib
  PATHS
  c:/ravi/lib
  ~/ravi/lib
)

set( NANOJITEXTRA_INCLUDE_DIRS "${NANOJITEXTRA_INCLUDE_DIR}" )
set( NANOJITEXTRA_LIBRARIES "${NANOJITEXTRA_LIBRARY}" )
