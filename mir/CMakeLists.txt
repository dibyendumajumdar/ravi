project(mir)

enable_language(C)

message(STATUS "OS type is ${CMAKE_SYSTEM_NAME}")
message(STATUS "System processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "Build type is ${CMAKE_BUILD_TYPE}")

set(MIR_HEADERS
    mir.h
    mir-gen.h
    mir-varr.h
    mir-dlist.h 
    mir-htab.h 
    mir-hash.h
    mir-bitmap.h
    )

set(MIR_SRCS 
    mir.c
    mir-gen.c
    )

set(C2MIR_SRCS
    c2mir/c2mir.c
    )

set(LIBS dl)

add_definitions(-DMIR_NO_IO=0)
add_definitions(-DMIR_NO_SCAN=1)

include_directories(".")
include_directories("./c2mir")

include(CheckCCompilerFlag)
check_c_compiler_flag("-fsigned-char" COMPILER_OPT_SIGNED_CHAR_SUPPORTED)
if (COMPILER_OPT_SIGNED_CHAR_SUPPORTED AND NOT CMAKE_C_FLAGS MATCHES "-fsigned-char")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsigned-char")
endif()
check_c_compiler_flag("-fno-tree-sra" COMPILER_OPT_NO_TREE_SRA_SUPPORTED)
if (COMPILER_OPT_NO_TREE_SRA_SUPPORTED AND NOT CMAKE_C_FLAGS MATCHES "-fno-tree-sra")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-tree-sra")
endif()
check_c_compiler_flag("-fno-ipa-cp-clone" COMPILER_OPT_NO_IPA_CP_CLONE_SUPPORTED)
if (COMPILER_OPT_NO_IPA_CP_CLONE_SUPPORTED AND NOT CMAKE_C_FLAGS MATCHES "-fno-ipa-cp-clone")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-ipa-cp-clone")
endif()


add_library(c2mir
    ${MIR_HEADERS}
    ${MIR_SRCS}
    ${C2MIR_SRCS})
target_link_libraries(c2mir ${LIBS})
set_property(TARGET c2mir PROPERTY C_STANDARD 11)
