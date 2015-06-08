cmake_minimum_required(VERSION 2.8)
project(Ravi)

enable_language(CXX)
enable_language(C)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

option(LLVM_JIT "Controls whether LLVM JIT compilation will be enabled" ON)
option(GCC_JIT "Controls whether GCC JIT compilation will be enabled" OFF)

if (LLVM_JIT)
  find_package(LLVM REQUIRED CONFIG)

  message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

  include_directories(${LLVM_INCLUDE_DIRS})
  add_definitions(${LLVM_DEFINITIONS})
  add_definitions(-DUSE_LLVM)
endif()

if (GCC_JIT)
  find_package(GCCJIT REQUIRED)

  message(STATUS "Found GCCJIT")

  include_directories(${GCCJIT_INCLUDE_DIRS})
  add_definitions(-DUSE_GCCJIT)
endif()


if (MSVC)
  set(CMAKE_C_FLAGS_DEBUG "/Od /D_DEBUG /MDd /Zi /RTC1 /EHsc")
  set(CMAKE_C_FLAGS_RELEASE "/DNDEBUG /O2 /MD /EHsc")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/DNDEBUG /O2 /MD /Zi /EHsc")
  set(CMAKE_C_FLAGS_MINSIZEREL "/DNDEBUG /O2 /MD /EHsc")    

  set(CMAKE_CXX_FLAGS "/EHsc")
  set(CMAKE_CXX_FLAGS_DEBUG "/Od /D_DEBUG /MDd /Zi /RTC1 /EHsc")
  set(CMAKE_CXX_FLAGS_RELEASE "/DNDEBUG /O2 /MD  /EHsc")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/DNDEBUG /O2 /MD /Zi /EHsc")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/DNDEBUG /O2 /MD /EHsc")  
  
  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /STACK:10000000")
  SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "/RELEASE /INCREMENTAL:NO /STACK:10000000")
  SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/DEBUG /RELEASE /INCREMENTAL:NO /STACK:10000000")
  SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/RELEASE /INCREMENTAL:NO /STACK:10000000")

  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TCMALLOC_LINK_FLAGS}")
  SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
  SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
  SET(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
  SET(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}")

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
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS_DEBUG "-std=c99 -O0 -g3 -fsanitize=address -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_LINUX")
  set(CMAKE_C_FLAGS_RELEASE "-std=c99 -O3 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_LINUX")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "-std=c99 -O1 -g3 -fsanitize=address -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_LINUX")
  set(CMAKE_CXX_FLAGS_RELEASE "-fno-rtti -O3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions -DLUA_COMPAT_5_2 -DLUA_USE_LINUX")
  set(CMAKE_CXX_FLAGS_DEBUG "-fno-rtti -O0 -fsanitize=address -g3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions -DLUA_COMPAT_5_2 -DLUA_USE_LINUX")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-fno-rtti -O1 -g3 -fsanitize=address -Wall -Wno-sign-compare -std=c++11 -fno-exceptions -DLUA_COMPAT_5_2 -DLUA_USE_LINUX")
endif()

if (APPLE)
  set(CMAKE_C_FLAGS "-std=c99 -O1 -g3 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_MACOSX")
  set(CMAKE_C_FLAGS_DEBUG "-std=c99 -O0 -g3 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_MACOSX")
  set(CMAKE_C_FLAGS_RELEASE "-std=c99 -O3 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_MACOSX")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "-std=c99 -O1 -g3 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_MACOSX")
  set(CMAKE_CXX_FLAGS "-O1 -g3 -Wall -fno-rtti -Wno-sign-compare -std=c++11 -fno-exceptions -DLUA_COMPAT_5_2 -DLUA_USE_MACOSX")
  set(CMAKE_CXX_FLAGS_RELEASE "-fno-rtti -O3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions -DLUA_COMPAT_5_2 -DLUA_USE_MACOSX")
  set(CMAKE_CXX_FLAGS_DEBUG "-fno-rtti -O0 -g3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions -DLUA_COMPAT_5_2 -DLUA_USE_MACOSX")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-fno-rtti -O1 -g3 -Wall -Wno-sign-compare -std=c++11 -fno-exceptions -DLUA_COMPAT_5_2 -DLUA_USE_MACOSX")
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")
add_definitions(-DLUA_COMPAT_MODULE -DCOCO_DISABLE)

# define LLVM JIT compiler sources
if (LLVM_JIT)
  set (LLVM_JIT_SRCS  src/ravi_llvmtypes.cpp 
     src/ravi_llvmcodegen.cpp src/ravi_llvmforprep.cpp src/ravi_llvmcomp.cpp
     src/ravi_llvmreturn.cpp src/ravi_llvmload.cpp src/ravi_llvmforloop.cpp
     src/ravi_llvmarith1.cpp src/ravi_llvmcall.cpp src/ravi_llvmtable.cpp
     src/ravi_llvmarith2.cpp src/ravi_llvmtforcall.cpp src/ravi_llvmrest.cpp)
endif()
# define the lua core source files
set (LUA_CORE_SRCS src/lapi.c src/lcode.c src/lctype.c src/ldebug.c src/ldo.c src/ldump.c 
     src/lfunc.c src/lgc.c src/llex.c src/lmem.c src/lobject.c src/lopcodes.c
     src/lparser.c src/lstate.c src/lstring.c src/ltable.c src/ltm.c src/lundump.c src/lcoco.c
     src/lvm.c src/lzio.c src/ravijit.cpp)
# define the lua lib source files
set (LUA_LIB_SRCS src/lauxlib.c src/lbaselib.c src/lbitlib.c src/lcorolib.c src/ldblib.c src/liolib.c
     src/lmathlib.c src/loslib.c src/ltablib.c src/lstrlib.c src/loadlib.c src/linit.c src/lutf8lib.c)

file(GLOB RAVI_HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")

if (MSVC OR APPLE)
  source_group("Ravi Headers" FILES ${RAVI_HEADERS})
  source_group("Ravi Source Files" FILES ${LUA_CORE_SRCS} ${LUA_LIB_SRCS})
  if (APPLE)
    set(EXTRA_LIBRARIES m readline)
  endif()
else()
  # On Linux we need to link libdl to get access to
  # functions like dlopen()
  set(EXTRA_LIBRARIES m dl readline)
endif()

#llvm_map_components_to_libnames(llvm_libs all)
#llvm_map_components_to_libnames(llvm_libs Support Core CodeGen AsmPrinter BitReader TransformUtils  
# InstCombine ScalarOpts Analysis ipa Object ExecutionEngine MC MCParser MCJIT ProfileData ipo
# RuntimeDyld Target X86CodeGen X86AsmParser X86Disassembler X86AsmPrinter X86Desc X86Info X86Utils AsmParser)

if (LLVM_JIT)
  llvm_map_components_to_libnames(llvm_libs 
    Analysis 
    Core 
    CodeGen 
    AsmParser 
    AsmPrinter 
    BitReader 
    ExecutionEngine 
    InstCombine 
    ipa 
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
    X86CodeGen 
    X86AsmParser 
    X86Disassembler 
    X86AsmPrinter 
    X86Desc 
    X86Info 
    X86Utils 
    )
  message(STATUS "llvm_libs ${llvm_libs}")
endif()


#Main library
add_library(ravi SHARED ${RAVI_HEADERS} ${LUA_LIB_SRCS} ${LUA_CORE_SRCS} ${LLVM_JIT_SRCS} ${GCC_JIT_SRCS})
if (MSVC)
  set_target_properties(ravi PROPERTIES DEFINE_SYMBOL "LUA_BUILD_AS_DLL")
endif()
target_link_libraries(ravi ${EXTRA_LIBRARIES} ${llvm_libs} ${GCCJIT_LIBRARIES})

#add_library(ravis ${RAVI_HEADERS} ${LUA_LIB_SRCS} ${LUA_CORE_SRCS})
#target_link_libraries(ravis ${EXTRA_LIBRARIES} ${llvm_libs})

add_executable(lua src/lua.c)
target_link_libraries(lua ravi)
    
#add_executable(luac src/luac.c)
#target_link_libraries(luac ravis)

if (LLVM_JIT)
  #VM test
  add_executable(test_vm tests/test_vm.c)
  target_link_libraries(test_vm ravi)

  #LLVM playground
  add_executable(test_llvm tests/test_llvm.cpp)
  target_link_libraries(test_llvm ravi)
endif()


if (GCC_JIT)
  #GCC JIT playground
  add_executable(test_gccjit tests/test_gccjit.c)
  target_link_libraries(test_gccjit ravi)
endif()

add_executable(test_misc tests/test_misc.c)

if (LLVM_JIT)
  add_test(TestVM test_vm)
  add_test(TestLLVM test_llvm)
endif()
add_test(TestMisc test_misc)
