cmake_minimum_required(VERSION 2.8)
project(Ravi)

enable_language(C)
enable_testing()

if (MSVC)
  set(CMAKE_C_FLAGS_DEBUG "/Od /D_DEBUG /MDd /Zi")
  set(CMAKE_C_FLAGS_RELEASE "/DNDEBUG /O2 /MD")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/DNDEBUG /O2 /MD /Zi")
  set(CMAKE_C_FLAGS_MINSIZEREL "/DNDEBUG /O1 /MD")    

  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG")
  SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "/RELEASE /INCREMENTAL:NO")
  SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/DEBUG /RELEASE /INCREMENTAL:NO")
  SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/RELEASE /INCREMENTAL:NO")

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
endif()

if (CMAKE_COMPILER_IS_GNUCXX OR APPLE)
  set(CMAKE_C_FLAGS "-std=c99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_LINUX")
  set(CMAKE_C_FLAGS_DEBUG "-std=c99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_LINUX")
  set(CMAKE_C_FLAGS_RELEASE "-std=c99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_LINUX")
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")
add_definitions(-DLUA_COMPAT_MODULE)

# define the lua core source files
set (LUA_CORE_SRCS src/lapi.c src/lcode.c src/lctype.c src/ldebug.c src/ldo.c src/ldump.c 
     src/lfunc.c src/lgc.c src/llex.c src/lmem.c src/lobject.c src/lopcodes.c
     src/lparser.c src/lstate.c src/lstring.c src/ltable.c src/ltm.c src/lundump.c
     src/lvm.c src/lzio.c)
# define the lua lib source files
set (LUA_LIB_SRCS src/lauxlib.c src/lbaselib.c src/lbitlib.c src/lcorolib.c src/ldblib.c src/liolib.c
     src/lmathlib.c src/loslib.c src/ltablib.c src/lstrlib.c src/loadlib.c src/linit.c src/lutf8lib.c)

if (MSVC)
  source_group("Ravi Source Files" FILES ${LUA_CORE_SRCS} ${LUA_LIB_SRCS})
endif(MSVC)

file(GLOB RAVI_HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")

if (MSVC)
  source_group("Ravi Headers" FILES ${RAVI_HEADERS})
endif()

if (MSVC)
else()
  # On Linux we need to link libdl to get access to
  # functions like dlopen()
  set(EXTRA_LIBRARIES m dl readline)
endif()

#Main library
add_library(ravi SHARED ${RAVI_HEADERS} ${LUA_LIB_SRCS} ${LUA_CORE_SRCS})
if (MSVC)
  set_target_properties(ravi PROPERTIES DEFINE_SYMBOL "LUA_BUILD_AS_DLL")
endif()
target_link_libraries(ravi ${EXTRA_LIBRARIES})

add_executable(lua src/lua.c)
target_link_libraries(lua ravi)
    
add_executable(luac src/luac.c)
target_link_libraries(luac ravi)

#VM test
add_executable(test_vm tests/test_vm.c)
target_link_libraries(test_vm ravi) 

add_test(TestVM test_vm)
