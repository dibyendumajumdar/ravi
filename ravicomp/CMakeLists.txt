cmake_minimum_required(VERSION 3.12)
project(RaviCompiler VERSION 0.0.1 LANGUAGES C)

option(ASAN "Controls whether address sanitizer should be enabled" OFF)

set(PUBLIC_HEADERS
    include/ravi_compiler.h
    include/ravi_api.h)

set(HEADERS
        ${PUBLIC_HEADERS}
        src/allocate.h
        src/ptrlist.h
        src/fnv_hash.h
        src/graph.h
        src/hash_table.h
        src/set.h
        src/membuf.h
        src/cfg.h
        src/linearizer.h
        src/common.h
        src/optimizer.h
        src/parser.h
        src/codegen.h
        src/chibicc/chibicc.h)

set(SRCS
        src/allocate.c
        src/ast_walker.c
        src/ast_simplify.c
        src/ast_lower.c
        src/ptrlist.c
        src/fnv_hash.c
        src/graph.c
        src/cfg.c
        src/hash_table.c
        src/set.c
        src/lexer.c
        src/parser.c
        src/ast_printer.c
        src/typechecker.c
        src/linearizer.c
        src/opt_unusedcode.c
        src/membuf.c
        src/codegen.c
        src/ravi_binding.c
        src/chibicc/chibicc_tokenize.c
        src/chibicc/chibicc_parse.c
        src/chibicc/chibicc_type.c
        src/chibicc/chibicc_strings.c
        src/chibicc/chibicc_unicode.c
        src/chibicc/chibicc_hashmap.c
        ../src/ravi_alloc.c
        )

message("SOURCE dir is ${RaviCompiler_SOURCE_DIR}")

if ($ENV{CLION_IDE})
    # CLion seems unable to handle include paths set on sources
    include_directories("${RaviCompiler_SOURCE_DIR}/include")
endif ()

if (WIN32)
    # disable warnings about C string functions
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

include(CheckCCompilerFlag)
if (NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wmissing-prototypes -Wstrict-prototypes -Werror=return-type")
    if (ASAN)
        set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
        check_c_compiler_flag("-fsanitize=address" COMPILER_ASAN_SUPPORTED)
        if (COMPILER_ASAN_SUPPORTED AND NOT CMAKE_C_FLAGS_DEBUG MATCHES "-fsanitize=address")
            set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
        endif ()
    endif ()
endif()

include(GNUInstallDirs)

set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

if (NOT WIN32)
    set(EXTRA_LIBRARIES m)
endif ()

if (WIN32)
    set(LIBTYPE STATIC)
else()
    set(LIBTYPE SHARED)
endif()
add_library(ravicomp ${LIBTYPE}
        ${HEADERS}
        ${SRCS})
target_include_directories(ravicomp
        PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
        PUBLIC "${RaviCompiler_SOURCE_DIR}/include"
        PRIVATE "${RaviCompiler_SOURCE_DIR}/src"
        PRIVATE "${RaviCompiler_SOURCE_DIR}/../src"
        )
target_link_libraries(ravicomp ${EXTRA_LIBRARIES})
set_property(TARGET ravicomp PROPERTY C_STANDARD 99)
include(GenerateExportHeader)
generate_export_header(ravicomp)

install(FILES ${PUBLIC_HEADERS}
        DESTINATION include/ravicomp)
install(TARGETS ravicomp
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT RaviCompiler_Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RaviCompiler_Development
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RaviCompiler_Runtime)
install(FILES
        ${PROJECT_BINARY_DIR}/ravicomp_export.h DESTINATION include/ravicomp
        )