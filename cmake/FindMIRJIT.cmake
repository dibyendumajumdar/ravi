find_path(MIRJIT_INCLUDE_DIR c2mir.h
  PATHS
  c:/Software/mir/include/mir
  ~/Software/mir/include/mir
  NO_DEFAULT_PATH
)

find_library(MIRJIT_LIBRARY
  NAMES libc2mir.a
  PATHS
  c:/Software/mir/lib
  ~/Software/mir/lib
)

set( MIRJIT_INCLUDE_DIRS "${MIRJIT_INCLUDE_DIR}" )
set( MIRJIT_LIBRARIES "${MIRJIT_LIBRARY}" )
