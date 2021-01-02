cmake_minimum_required(VERSION 3.12)
project(mir LANGUAGES C)

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

set(LIBS ${CMAKE_DL_LIBS})
if (WIN32)
    set(MIR_BUILD_TYPE STATIC)
else()
    set(MIR_BUILD_TYPE SHARED)
endif()

#add_definitions(-DMIR_NO_IO=0)
#add_definitions(-DMIR_NO_SCAN=1)
set_property(SOURCE ${MIR_SRCS} ${C2MIR_SRCS} ${MIR_JIT_SRCS}
        APPEND
        PROPERTY COMPILE_DEFINITIONS "MIR_NO_IO=0;MIR_NO_SCAN=1;MIR_NO_INTERP=1")

if ($ENV{CLION_IDE})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/c2mir)
endif()

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

add_library(c2mir ${MIR_BUILD_TYPE}
    ${MIR_HEADERS}
    ${MIR_SRCS}
    ${C2MIR_SRCS})
target_link_libraries(c2mir ${LIBS})
target_include_directories(c2mir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/c2mir)
set_property(TARGET c2mir PROPERTY C_STANDARD 11)

install(TARGETS c2mir
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT mir_Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT mir_Development
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT mir_Runtime)