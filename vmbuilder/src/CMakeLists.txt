# Copyright (C) 2007-2013 LuaDist.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( vmbuilder C )
cmake_minimum_required ( VERSION 2.8 )

if ( APPLE )
	list ( APPEND LIBS m )
elseif ( NOT WIN32 )
	list ( APPEND LIBS m )
endif ()

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Build minilua
add_executable ( minilua minilua.c )
target_link_libraries ( minilua ${LIBS} )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dynasm)

# Dynasm
set ( DASM ${CMAKE_CURRENT_SOURCE_DIR}/../dynasm/dynasm.lua )
set ( DASM_T ${CMAKE_CURRENT_SOURCE_DIR}/buildvm_arch.h )

# Windows is ... special
if ( WIN32 )
	list ( APPEND DASM_FLAGS -D WIN )
endif ()

list ( APPEND DASM_FLAGS -D P64 )

set ( DASM_DASC ${CMAKE_CURRENT_SOURCE_DIR}/vm_x64.dasc )

# Generate buildvm arch header
add_custom_command(OUTPUT ${DASM_T}
	COMMAND minilua ${DASM} -MF ${DASM_FLAGS} -o ${DASM_T} ${DASM_DASC}
	DEPENDS minilua ${DASM_DASC}
)

set(HEADERS
	../../include/ravi_arch.h
	../../include/ravi_def.h
	../dynasm/dasm_proto.h
	../dynasm/dasm_x86.h)

set(SRCS 
	buildvm.c
	buildvm_asm.c
	buildvm_peobj.c
	)

# Buildvm
add_executable ( buildvm ${SRCS} ${HEADERS} ${DASM_T} )

install(TARGETS buildvm
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../bin)