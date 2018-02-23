cmake_minimum_required(VERSION 2.8)
project(Ravi)

enable_language(CXX)
enable_language(C)
enable_language(ASM)
enable_testing()

# By default JIT is OFF
option(LLVM_JIT "Controls whether LLVM JIT compilation will be enabled, default is OFF" OFF)
option(STATIC_BUILD "Build static version of Ravi, default is OFF" OFF)
option(EMBEDDED_DMRC "Controls whether the embedded dmrC feature should be enabled, default is OFF" OFF)
option(COMPUTED_GOTO "Controls whether the interpreter switch will use computed gotos on gcc/clang, default is OFF" ON)
option(ASM_VM "Controls whether to use the new VM (not ready yet! so don't turn on)" OFF)
option(LTESTS "Controls whether ltests are enabled in Debug mode" OFF)

if (LLVM_JIT)
    set(ASM_VM OFF)
endif()

if (ASM_VM)
    # For now we switch to static build 
    # TODO A fix is needed to ensure that in shared library the asm functions are resolved
    set(STATIC_BUILD ON)
    set(LTESTS OFF)
endif()

if (STATIC_BUILD)
    message(STATUS "STATIC library build enabled")
else()
    message(STATUS "DYNAMIC library build enabled")
endif()

if (LLVM_JIT)
    find_package(LLVM REQUIRED CONFIG)

    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})

    # We also need to define USE_LLVM when compiling code
    # but rather than setting globally we set this when building the
    # library 
endif()

if (EMBEDDED_DMRC)
    add_definitions(-DUSE_DMR_C)
endif()

message(STATUS "Computed goto ${COMPUTED_GOTO}")
if (COMPUTED_GOTO AND MSVC)
    message(WARNING "Computed goto is not available with MSVC")
endif()

if (NOT LLVM_JIT)
    message(WARNING "LLVM will not be enabled; specify -DLLVM_JIT=ON to enable")
endif()

if (MSVC)
    set(CMAKE_C_FLAGS_DEBUG "/Od /D_DEBUG /MDd /Zi /RTC1 /EHsc")
    set(CMAKE_C_FLAGS_RELEASE "/DNDEBUG /O2 /MD /EHsc")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/DNDEBUG /O2 /MD /Zi /EHsc")
    set(CMAKE_C_FLAGS_MINSIZEREL "/DNDEBUG /O2 /MD /EHsc")

    set(CMAKE_CXX_FLAGS "/EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /D_DEBUG /MDd /Zi /RTC1 /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "/DNDEBUG /O2 /MD /EHsc")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/DNDEBUG /O2 /MD /Zi /EHsc")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "/DNDEBUG /O2 /MD /EHsc")

    # Disable signed/unsigned int conversion warnings.
    add_definitions("/wd4018")
    # Disable warning about using struct/class for the same symobl.
    add_definitions("/wd4099")
    # Disable performance warning about int-to-bool conversion.
    add_definitions("/wd4800")
    # Disable signed/unsigned int conversion warnings.
    add_definitions("/wd4018")
    # Disable warning about the insecurity of using "std::copy".
    add_definitions("/wd4996")
    add_definitions("/wd4291")
    add_definitions("/wd4624")
    add_definitions("/wd4141")
    add_definitions("/DLUA_COMPAT_5_2")
    add_definitions("/DLUA_COMPAT_5_1")
elseif ((CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang") AND NOT APPLE)
    if (NOT WIN32)
        # assume Linux
        set(OS_FLAGS "-DLUA_USE_LINUX")
    endif ()
    # set(SANITIZER_FLAGS "-fsanitize=address")
    set(LUA_COMPAT_FLAGS "-DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1")
    set(CMAKE_C_FLAGS "-std=c99 -O2 -fomit-frame-pointer -Wall -Wextra -Winline ${LUA_COMPAT_FLAGS} ${OS_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "${SANITIZER_FLAGS} -fno-omit-frame-pointer -std=c99 -O0 -g3 -Wall -Wextra ${LUA_COMPAT_FLAGS} ${OS_FLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "-std=c99 -O2 -fomit-frame-pointer -Wall -Wextra -Winline ${LUA_COMPAT_FLAGS} ${OS_FLAGS}")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${SANITIZER_FLAGS} -std=c99 -O1 -g3 -Wall -Wextra ${LUA_COMPAT_FLAGS} ${OS_FLAGS}")
    set(CMAKE_CXX_FLAGS "-fno-rtti -O2 -fomit-frame-pointer -Wall -Wno-sign-compare -Winline -std=c++11 -fno-exceptions ${LUA_COMPAT_FLAGS} ${OS_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "-fno-rtti -O2 -fomit-frame-pointer -Wall -Wno-sign-compare -Winline -std=c++11 -fno-exceptions ${LUA_COMPAT_FLAGS} ${OS_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${SANITIZER_FLAGS} -fno-omit-frame-pointer -fno-rtti -O0 -g3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions ${LUA_COMPAT_FLAGS} ${OS_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${SANITIZER_FLAGS} -fno-rtti -O1 -g3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions ${LUA_COMPAT_FLAGS} ${OS_FLAGS}")
elseif (APPLE)
    set(LUA_COMPAT_FLAGS "-DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1")
    set(CMAKE_C_FLAGS "-std=c99 -O3 -Wall -Wextra ${LUA_COMPAT_FLAGS} -DLUA_USE_MACOSX")
    set(CMAKE_C_FLAGS_DEBUG "-std=c99 -O0 -g3 -Wall -Wextra ${LUA_COMPAT_FLAGS} -DLUA_USE_MACOSX")
    set(CMAKE_C_FLAGS_RELEASE "-std=c99 -O3 -Wall -Wextra ${LUA_COMPAT_FLAGS} -DLUA_USE_MACOSX")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-std=c99 -O1 -g3 -Wall -Wextra ${LUA_COMPAT_FLAGS} -DLUA_USE_MACOSX")
    set(CMAKE_CXX_FLAGS "-O3 -Wall -fno-rtti -Wno-sign-compare -std=c++11 -fno-exceptions ${LUA_COMPAT_FLAGS} -DLUA_USE_MACOSX")
    set(CMAKE_CXX_FLAGS_RELEASE "-fno-rtti -O3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions ${LUA_COMPAT_FLAGS} -DLUA_USE_MACOSX")
    set(CMAKE_CXX_FLAGS_DEBUG "-fno-rtti -O0 -g3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions ${LUA_COMPAT_FLAGS} -DLUA_USE_MACOSX")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-fno-rtti -O1 -g3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions ${LUA_COMPAT_FLAGS} -DLUA_USE_MACOSX")
endif ()

include_directories("${PROJECT_SOURCE_DIR}/include")
if ((CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "DEBUG") AND LTESTS AND NOT ASM_VM)
    # Note that enabling ltests.h messes with global_State and thus interferes with ASM_VM
    message(STATUS "Enabling Lua extended test harness 'ltests'")
    add_definitions(-DLUA_USER_H="ltests.h")
endif ()

if (ASM_VM)
	# For now we switch to static build 
	# TODO A fix is needed to ensure that in shared library the asm functions are resolved
    add_definitions(-DRAVI_USE_ASMVM)
    set ( ASMVM_DEFS ${PROJECT_SOURCE_DIR}/include/ravi_asmvm_defs.h )
    if (WIN32 AND NOT CYGWIN)
        set(VMMODE peobj)
    elseif (APPLE)
        set(VMMODE machasm)
    else()
        set(VMMODE elfasm)
    endif()      
  
	# This macro runs the buildvm command to generate the VM code
    macro(add_buildvm_target _target _mode)
        add_custom_command(OUTPUT ${_target}
            COMMAND ${PROJECT_SOURCE_DIR}/vmbuilder/bin/buildvm ARGS -m ${_mode} -o ${_target} ${ARGN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${PROJECT_SOURCE_DIR}/vmbuilder/bin/buildvm ${ARGN}
        )
    endmacro(add_buildvm_target)

	# Generate ravi_asmvm.obj / ravi_asmvm.s (obj on Windows)
    if ( WIN32 AND NOT CYGWIN )
        add_buildvm_target ( ${CMAKE_CURRENT_BINARY_DIR}/ravi_asmvm.obj ${VMMODE} )
        set (ASMVM_SRC ${CMAKE_CURRENT_BINARY_DIR}/ravi_asmvm.obj)
    else ()
        add_buildvm_target ( ${CMAKE_CURRENT_BINARY_DIR}/ravi_asmvm.s ${VMMODE} )
        set (ASMVM_SRC ${CMAKE_CURRENT_BINARY_DIR}/ravi_asmvm.s)
    endif ()

	# Generate the ravi_bcdef.h header file
    add_buildvm_target ( ${ASMVM_DEFS}  bcdef  ${LJLIB_C} )

    SET (ASMVM_DEPS
        ${ASMVM_SRC}
        ${ASMVM_DEFS}
    )
endif()

# define LLVM JIT compiler sources
if (LLVM_JIT)
    set(LLVM_JIT_SRCS src/ravi_llvmjit.cpp src/ravi_llvmtypes.cpp
            src/ravi_llvmcodegen.cpp src/ravi_llvmforprep.cpp src/ravi_llvmcomp.cpp
            src/ravi_llvmreturn.cpp src/ravi_llvmload.cpp src/ravi_llvmforloop.cpp
            src/ravi_llvmarith1.cpp src/ravi_llvmcall.cpp src/ravi_llvmtable.cpp
            src/ravi_llvmarith2.cpp src/ravi_llvmtforcall.cpp src/ravi_llvmrest.cpp
            src/ravi_llvmluaapi.cpp)
else()    
    set(NO_JIT_SRCS src/ravi_nojit.c)
endif()
# define the lua core source files
set(LUA_CORE_SRCS src/lapi.c src/lcode.c src/lctype.c src/ldebug.c src/ldo.c src/ldump.c
        src/lfunc.c src/lgc.c src/llex.c src/lmem.c src/lobject.c src/lopcodes.c
        src/lparser.c src/lstate.c src/lstring.c src/ltable.c src/ltm.c src/lundump.c
        src/lvm.c src/lzio.c src/ravijit.cpp src/ltests.c src/ravi_profile.c src/ravi_membuf.c
        src/ravi_jitshared.c src/bit.c)
if (COMPUTED_GOTO AND NOT MSVC)
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        set_source_files_properties(src/lvm.c PROPERTIES COMPILE_FLAGS -DRAVI_USE_COMPUTED_GOTO)
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
        set_source_files_properties(src/lvm.c PROPERTIES COMPILE_FLAGS "-fno-crossjumping -fno-gcse -DRAVI_USE_COMPUTED_GOTO")
    endif()
endif()      
# define the lua lib source files
set(LUA_LIB_SRCS src/lauxlib.c src/lbaselib.c src/lbitlib.c src/lcorolib.c src/ldblib.c src/liolib.c
        src/lmathlib.c src/loslib.c src/ltablib.c src/lstrlib.c src/loadlib.c src/linit.c src/lutf8lib.c)

file(GLOB RAVI_HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
set(LUA_HEADERS include/lua.h include/luaconf.h include/lualib.h include/lauxlib.h)

if (MSVC OR APPLE)
    source_group("Ravi Headers" FILES ${RAVI_HEADERS})
    source_group("Ravi Source Files" FILES ${LUA_CORE_SRCS} ${LUA_LIB_SRCS}
            ${LLVM_JIT_SRCS})
    if (APPLE)
        set(EXTRA_LIBRARIES m readline)
    endif ()
elseif (NOT WIN32)
    # On Linux we need to link libdl to get access to
    # functions like dlopen()
    # ubsan can be added to get -fsanitize=undefined
    set(EXTRA_LIBRARIES m dl readline)
endif ()

if (EMBEDDED_DMRC)
    if (CMAKE_COMPILER_IS_GNUCC)
        execute_process(COMMAND ${CMAKE_C_COMPILER} --print-file-name=
            OUTPUT_VARIABLE GCC_BASE OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND ${CMAKE_C_COMPILER} -print-multiarch
            OUTPUT_VARIABLE MULTIARCH_TRIPLET ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

        add_definitions(-DGCC_BASE="${GCC_BASE}")
        add_definitions(-DMULTIARCH_TRIPLET="${MULTIARCH_TRIPLET}")
    endif()

    message( STATUS "GCC_BASE_DIR      : " ${GCC_BASE})
    message( STATUS "MULTIARCH_TRIPLET : " ${MULTIARCH_TRIPLET} )

    include_directories("${PROJECT_SOURCE_DIR}/dmr_c/src")

    set(DMR_C_HEADERS 
        dmr_c/src/allocate.h
        dmr_c/src/char.h
        dmr_c/src/expression.h
        dmr_c/src/flow.h
        dmr_c/src/ident-list.h
        dmr_c/src/linearize.h
        dmr_c/src/lib.h        
        dmr_c/src/parse.h
        dmr_c/src/port.h 
        dmr_c/src/ptrlist.h
        dmr_c/src/ptrmap.h        
        dmr_c/src/scope.h
        dmr_c/src/symbol.h
        dmr_c/src/target.h
        dmr_c/src/token.h 
        dmr_c/src/ssa.h 
        dmr_c/src/walksymbol.h         
        )

    set(DMR_C_SRCS 
        dmr_c/src/allocate.c
        dmr_c/src/builtin.c
        dmr_c/src/char.c
        dmr_c/src/cse.c
        dmr_c/src/expression.c
        dmr_c/src/evaluate.c
        dmr_c/src/expand.c
        dmr_c/src/flow.c
        dmr_c/src/inline.c
        dmr_c/src/lib.c
        dmr_c/src/linearize.c
        dmr_c/src/liveness.c
        dmr_c/src/memops.c
        dmr_c/src/parse.c
        dmr_c/src/target.c
        dmr_c/src/tokenize.c
        dmr_c/src/pre-process.c
        dmr_c/src/ptrlist.c
        dmr_c/src/ptrmap.c
        dmr_c/src/scope.c
        dmr_c/src/show-parse.c
        dmr_c/src/simplify.c
        dmr_c/src/symbol.c
        dmr_c/src/unssa.c
        dmr_c/src/ssa.c
        dmr_c/src/walksymbol.c
        src/ravi_dmrc_parsesymbols.c
        )

    if (LLVM_JIT)
        set(DMR_C_JIT_SRCS
            dmr_c/llvm-backend/sparse-llvm.c
            )
        set(DMR_C_JIT_HEADERS
            dmr_c/llvm-backend/dmr_c.h            
            )
        include_directories("${PROJECT_SOURCE_DIR}/dmr_c/llvm-backend")
    endif()  
    
    if (MSVC OR APPLE)
        source_group("dmrC Headers" FILES ${DMR_C_HEADERS} ${DMR_C_JIT_HEADERS})
        source_group("dmrC Source Files" FILES ${DMR_C_SRCS} ${DMR_C_JIT_SRCS})
    endif()
endif()


# Enable minimal required LLVM components so that the
# the size of the resulting binary is manageable
if (LLVM_JIT)
    if (${LLVM_PACKAGE_VERSION} VERSION_LESS "3.8")
        set(LLVM_EXTRA_LIBS ipa)
    endif()
	if (NOT ${LLVM_PACKAGE_VERSION} VERSION_LESS "5.0.0")
		set(LLVM_EXTRA_LIBS orcjit)
	endif()
    message(STATUS "SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR}")
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
	set(LLVM_LIBS_PROCESSOR
		X86CodeGen
		X86AsmParser
		X86Disassembler
		X86AsmPrinter
		X86Desc
		X86Info
		X86Utils
	    )
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
	set(LLVM_LIBS_PROCESSOR
		ARMCodeGen
		ARMAsmParser
		ARMDisassembler
		ARMAsmPrinter
		ARMDesc
		ARMInfo
	    )
    endif()
    llvm_map_components_to_libnames(LLVM_LIBS
            Analysis
            AsmParser
            AsmPrinter
            BitReader
            Core
            CodeGen
            ExecutionEngine
            InstCombine
            ${LLVM_EXTRA_LIBS}
            ipo
            MC
            MCJIT
            MCParser
            Object
            RuntimeDyld
            ScalarOpts
            Support
            Target
            TransformUtils
            ${LLVM_LIBS_PROCESSOR}
            )
    message(STATUS "LLVM_LIBS ${LLVM_LIBS}")
endif ()

if (NOT STATIC_BUILD)
    set(LIBRAVI_BUILD_TYPE SHARED)
endif()

if (LLVM_JIT)
    set (LIBRAVI_NAME libravillvm) 
else()
    set (LIBRAVI_NAME libravinojit)
endif()

#Main library
add_library(${LIBRAVI_NAME} ${LIBRAVI_BUILD_TYPE} 
        ${RAVI_HEADERS}
        ${LUA_LIB_SRCS} 
        ${LUA_CORE_SRCS}
        ${LLVM_JIT_SRCS}   
        ${NO_JIT_SRCS}
        ${DMR_C_HEADERS}
        ${DMR_C_SRCS}
        ${DMR_C_JIT_SRCS}
        ${ASMVM_DEPS}
        )
if (NOT STATIC_BUILD)
    if (WIN32)
      # enable DLL export
      set_target_properties(${LIBRAVI_NAME} PROPERTIES DEFINE_SYMBOL "LUA_BUILD_AS_DLL")
  else()
      set_target_properties(${LIBRAVI_NAME} PROPERTIES PREFIX "")
  endif ()
endif()
if (LLVM_JIT)
    set_target_properties(${LIBRAVI_NAME} PROPERTIES COMPILE_DEFINITIONS "USE_LLVM=1")
endif()
target_link_libraries(${LIBRAVI_NAME} ${EXTRA_LIBRARIES} ${LLVM_LIBS})

# Ravi executable
add_executable(ravi src/lua.c)
target_link_libraries(ravi ${LIBRAVI_NAME})

set(NOJIT_RAVI_SRCS 
    ${RAVI_HEADERS}
    ${LUA_LIB_SRCS} 
    ${LUA_CORE_SRCS}
    src/ravi_nojit.c
    )

# Create a simple NoJIT version of statically linked ravi
# This is sometimes useful in other projects that just need a Lua commandline
# but do not care about the shared library 
add_executable(ravi_s 
    src/lua.c
    ${NOJIT_RAVI_SRCS}
)
target_link_libraries(ravi_s ${EXTRA_LIBRARIES})

# Simple VM tests
add_executable(test_vm tests/test_vm.c)
target_link_libraries(test_vm ${LIBRAVI_NAME})

if (LLVM_JIT)
    # LLVM playground
    add_executable(test_llvm tests/test_llvm.cpp)
    set_target_properties(test_llvm PROPERTIES COMPILE_DEFINITIONS "USE_LLVM=1")
    target_link_libraries(test_llvm ${LIBRAVI_NAME})
endif ()

add_executable(test_misc tests/test_misc.c)

if (LLVM_JIT)
    add_test(TestLLVM test_llvm)
endif ()
add_test(TestVM test_vm)
add_test(TestMisc test_misc)

# Build VSCode Debug Adapter for Ravi  
add_executable(testravidebug 
    vscode-debugger/src/testravidebug.c 
    vscode-debugger/src/json.c 
    vscode-debugger/src/protocol.c
    ${NOJIT_RAVI_SRCS})
target_link_libraries(testravidebug ${EXTRA_LIBRARIES})

add_test(TestRaviDebug testravidebug)

set(RAVI_DEBUGGER_TARGET ravidebug)

# Ravi VSCode Debug adapter
add_executable(${RAVI_DEBUGGER_TARGET} 
    vscode-debugger/src/ravidebug.c 
    vscode-debugger/src/json.c 
    vscode-debugger/src/protocol.c
    ${NOJIT_RAVI_SRCS})
target_link_libraries(${RAVI_DEBUGGER_TARGET} ${EXTRA_LIBRARIES})

install(FILES ${LUA_HEADERS}
        DESTINATION include/ravi)
install(TARGETS ${LIBRAVI_NAME} ravi ${RAVI_DEBUGGER_TARGET} ravi_s
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)
