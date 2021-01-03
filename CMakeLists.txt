cmake_minimum_required(VERSION 3.12)
project(Ravi VERSION 1.0.4 LANGUAGES C)

# By default MIR JIT backend is automatically enabled. To disable user must specify
# NO_JIT=ON

option(NO_JIT "Controls whether JIT should be disabled, default is OFF" OFF)
option(STATIC_BUILD "Build static version of Ravi, default is OFF" OFF)
option(COMPUTED_GOTO "Controls whether the interpreter switch will use computed gotos on gcc/clang, default is ON" ON)
option(LTESTS "Controls whether ltests are enabled in Debug mode; note requires Debug build" ON)
option(ASAN "Controls whether address sanitizer should be enabled" OFF)
option(RAVICOMP "Controls whether to link in RaviComp" ON)

# By we enable MIR JIT
if (NOT NO_JIT)
    set(MIR_JIT ON)
endif ()

if (MIR_JIT)
    set(STATIC_BUILD OFF) # Because we need to expose the symbols in the library
endif ()

if (STATIC_BUILD)
    message(STATUS "STATIC library build enabled")
    set(LIBRAVI_BUILD_TYPE STATIC)
else ()
    message(STATUS "DYNAMIC library build enabled")
    set(LIBRAVI_BUILD_TYPE SHARED)
endif ()
message(STATUS "Computed goto ${COMPUTED_GOTO}")
if (COMPUTED_GOTO AND MSVC)
    message(WARNING "Computed goto is not available with MSVC")
endif ()

include_directories("${PROJECT_SOURCE_DIR}/include")

# define the Lua core source files
set(LUA_CORE_SRCS src/lapi.c src/lcode.c src/lctype.c src/ldebug.c src/ldo.c src/ldump.c
        src/lfunc.c src/lgc.c src/llex.c src/lmem.c src/lobject.c src/lopcodes.c
        src/lparser.c src/lstate.c src/lstring.c src/ltable.c src/ltm.c src/lundump.c
        src/lvm.c src/lzio.c src/ravi_jit.c src/ltests.c src/ravi_profile.c
        src/ravi_membuf.c src/ravi_jitshared.c src/bit.c src/ravi_alloc.c)
# define the Lua library source files
set(LUA_LIB_SRCS src/lauxlib.c src/lbaselib.c src/lbitlib.c src/lcorolib.c src/ldblib.c src/liolib.c
        src/lmathlib.c src/loslib.c src/ltablib.c src/lstrlib.c src/loadlib.c src/linit.c src/lutf8lib.c)
set(LUA_HEADERS include/lua.h include/luaconf.h include/lualib.h include/lauxlib.h)
set(MIR_JIT_SRCS src/ravi_mirjit.c)
set(NO_JIT_SRCS src/ravi_nojit.c)
set(LUA_CMD_SRCS src/lua.c)
set(RAVICOMP_SRCS src/ravi_complib.c)
file(GLOB RAVI_HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")

if (COMPUTED_GOTO AND NOT MSVC)
    if (CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "AppleClang")
        set_source_files_properties(src/lvm.c PROPERTIES COMPILE_FLAGS -DRAVI_USE_COMPUTED_GOTO)
    elseif (CMAKE_C_COMPILER_ID MATCHES "GNU")
        set_source_files_properties(src/lvm.c PROPERTIES COMPILE_FLAGS "-fno-crossjumping -fno-gcse -DRAVI_USE_COMPUTED_GOTO")
    endif ()
endif ()

include(CheckCCompilerFlag)
check_c_compiler_flag("-march=native" COMPILER_OPT_ARCH_NATIVE_SUPPORTED)
if (COMPILER_OPT_ARCH_NATIVE_SUPPORTED AND NOT CMAKE_C_FLAGS MATCHES "-march=")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif ()
if (ASAN)
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
    check_c_compiler_flag("-fsanitize=address" COMPILER_ASAN_SUPPORTED)
    if (COMPILER_ASAN_SUPPORTED AND NOT CMAKE_C_FLAGS_DEBUG MATCHES "-fsanitize=address")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
    endif ()
endif ()

if (MIR_JIT)
    message(STATUS "MIRJIT enabled")
    add_subdirectory(mir)
    set(MIRJIT_LIBRARIES c2mir)
    set(JIT_SRCS ${MIR_JIT_SRCS})
else ()
    set(JIT_SRCS ${NO_JIT_SRCS})
endif ()

if (RAVICOMP)
    set(ADDON_SRCS ${RAVICOMP_SRCS})
    add_subdirectory(ravicomp)
    set(RAVICOMP_LIBRARIES ravicomp)
endif ()

# IDE stuff
if (MSVC OR APPLE)
    source_group("Ravi Headers" FILES ${RAVI_HEADERS})
    source_group("Ravi Source Files" FILES ${LUA_CORE_SRCS} ${LUA_LIB_SRCS} ${JIT_SRCS} ${ADDON_SRCS})
endif ()

# Misc setup
if (MSVC OR APPLE)
    if (APPLE)
        set(EXTRA_LIBRARIES m readline)
    endif ()
elseif (NOT WIN32)
    # On Linux we need to link libdl to get access to
    # functions like dlopen()
    # ubsan can be added to get -fsanitize=undefined
    set(EXTRA_LIBRARIES m dl readline)
endif ()

set(LIBRAVI_NAME libravi)

#Main library
add_library(${LIBRAVI_NAME} ${LIBRAVI_BUILD_TYPE}
        ${RAVI_HEADERS}
        ${LUA_LIB_SRCS}
        ${LUA_CORE_SRCS}
        ${JIT_SRCS}
        ${ADDON_SRCS})
target_link_libraries(${LIBRAVI_NAME} LINK_PUBLIC ${EXTRA_LIBRARIES} ${MIRJIT_LIBRARIES} ${RAVICOMP_LIBRARIES})

# Main Ravi executable
add_executable(ravi ${LUA_CMD_SRCS})
target_link_libraries(ravi ${LIBRAVI_NAME})

# Sources that are needed for a static NOJIT basic library
set(NOJIT_RAVI_SRCS
        ${RAVI_HEADERS}
        ${LUA_LIB_SRCS}
        ${LUA_CORE_SRCS}
        ${NO_JIT_SRCS})
set(RAVI_STATICEXEC_TARGET ravi_s)

# We always build a static library without JIT so that
# we can create some default executables
add_library(libravinojit_static
        ${NOJIT_RAVI_SRCS})
set_target_properties(libravinojit_static PROPERTIES PREFIX "") # As we already prefix with lib
target_link_libraries(libravinojit_static ${EXTRA_LIBRARIES})

# Create a simple NoJIT version of statically linked ravi
# This is sometimes useful in other projects that just need a Lua commandline
# but do not care about the shared library
add_executable(${RAVI_STATICEXEC_TARGET}
        ${LUA_CMD_SRCS})
target_link_libraries(ravi_s libravinojit_static)

if (NOT LTESTS)
    # Note that enabling ltests.h messes with global_State
    message(STATUS "Disabling Lua extended test harness 'ltests'")
    set_property(
            TARGET ${LIBRAVI_NAME} ravi
            APPEND
            PROPERTY COMPILE_DEFINITIONS NO_LUA_DEBUG)
    set(NO_LUA_DEBUG 1)
endif ()
if (MIR_JIT)
    set_property(
            TARGET ${LIBRAVI_NAME} ravi
            APPEND
            PROPERTY COMPILE_DEFINITIONS "USE_MIRJIT=1")
    set(USE_MIRJIT 1)
endif ()
if (NOT STATIC_BUILD)
    if (WIN32)
        # enable DLL export
        set_property(
                TARGET ${LIBRAVI_NAME}
                APPEND
                PROPERTY COMPILE_DEFINITIONS LUA_BUILD_AS_DLL)
    else ()
        set_target_properties(${LIBRAVI_NAME} PROPERTIES PREFIX "")
    endif ()
else ()
    set_target_properties(${LIBRAVI_NAME} PROPERTIES PREFIX "")
endif ()
if (RAVICOMP)
    set_property(
            TARGET ${LIBRAVI_NAME}
            APPEND
            PROPERTY COMPILE_DEFINITIONS "USE_RAVICOMP=1")
    set(USE_RAVICOMP 1)
endif ()
if (APPLE)
    set_property(
            TARGET ${LIBRAVI_NAME} libravinojit_static
            APPEND
            PROPERTY COMPILE_DEFINITIONS "LUA_USE_MACOSX=1")
elseif (UNIX)
    set_property(
            TARGET ${LIBRAVI_NAME} libravinojit_static
            APPEND
            PROPERTY COMPILE_DEFINITIONS "LUA_USE_LINUX=1")
endif ()

include(GNUInstallDirs)
configure_file(ravi-config.h.in ravi-config.h @ONLY)
target_include_directories(${LIBRAVI} ravi
        PUBLIC
        "${PROJECT_BINARY_DIR}")

# Ravi VSCode Debug adapter
set(RAVI_DEBUGGER_TARGET ravidebug)
add_executable(${RAVI_DEBUGGER_TARGET}
        vscode-debugger/src/ravidebug.c
        vscode-debugger/src/json.c
        vscode-debugger/src/protocol.c)
target_link_libraries(${RAVI_DEBUGGER_TARGET} libravinojit_static)

# Tests for VSCode Debug Adapter
add_executable(testravidebug
        vscode-debugger/src/testravidebug.c
        vscode-debugger/src/json.c
        vscode-debugger/src/protocol.c)
target_link_libraries(testravidebug libravinojit_static)

configure_file(lua-config.cmake.in lua-config.cmake @ONLY)

if (WIN32)
    configure_file(ravi-env.bat.in ravi-env.bat @ONLY)
    set(RAVI_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/ravi-env.bat)
elseif (APPLE)
    configure_file(ravi-env.osx.sh.in ravi-env.sh @ONLY)
    set(RAVI_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/ravi-env.sh)
else ()
    configure_file(ravi-env.linux.sh.in ravi-env.sh @ONLY)
    set(RAVI_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/ravi-env.sh)
endif ()

install(FILES ${LUA_HEADERS}
        DESTINATION include/ravi)
install(TARGETS ${LIBRAVI_NAME} ravi ${RAVI_DEBUGGER_TARGET} ${RAVI_STATICEXEC_TARGET}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Ravi_Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Ravi_Development
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Ravi_Runtime)
install(FILES ${RAVI_SCRIPTS}
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lua-config.cmake
        DESTINATION cmake)
