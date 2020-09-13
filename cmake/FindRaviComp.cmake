find_path(RAVICOMP_INCLUDE_DIRS ravi_compiler.h
    PATHS
    c:/Software/ravicomp/include/ravicomp
    ~/Software/ravicomp/include/ravicomp
    NO_DEFAULT_PATH
  )

find_library(RAVICOMP_LIBRARIES
    NAMES ravicomp
    PATHS
    c:/Software/ravicomp/lib
    ~/Software/ravicomp/lib
    ~/Software/ravicomp/lib64
  )

