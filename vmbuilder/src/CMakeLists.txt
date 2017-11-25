# This CMake script is based on the LuaJIT one created by:
# Copyright (C) 2007-2013 LuaDist.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( vmbuilder C )
cmake_minimum_required ( VERSION 2.8 )

set( RAVI_INCDIR ${CMAKE_CURRENT_SOURCE_DIR}/../../include )
set( DASM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../dynasm )
set( BINDIR ${CMAKE_CURRENT_SOURCE_DIR}/../bin )
set( SRCDIR ${CMAKE_CURRENT_SOURCE_DIR} )

# Dynasm
set ( DASM ${DASM_DIR}/dynasm.lua )
set ( DASM_T ${CMAKE_CURRENT_SOURCE_DIR}/buildvm_arch.h )
set ( DASM_DASC ${CMAKE_CURRENT_SOURCE_DIR}/vm_x64.dasc )

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

include_directories(${RAVI_INCDIR})
include_directories(${DASM_DIR})

# Windows is ... special
if ( WIN32 )
	list ( APPEND DASM_FLAGS -D WIN )
endif ()

list ( APPEND DASM_FLAGS -D P64 )

# Generate buildvm arch header
add_custom_command(OUTPUT ${DASM_T}
	COMMAND minilua ${DASM} -MF ${DASM_FLAGS} -o ${DASM_T} ${DASM_DASC}
	DEPENDS minilua ${DASM_DASC}
)

set(HEADERS
	${RAVI_INCDIR}/ravi_arch.h
	${RAVI_INCDIR}/ravi_def.h
	${DASM_DIR}/dasm_proto.h
	${DASM_DIR}/dasm_x86.h)

set(SRCS 
	${SRCDIR}/buildvm.c
	${SRCDIR}/buildvm_asm.c
	${SRCDIR}/buildvm_peobj.c
	)

# Buildvm
add_executable ( buildvm ${SRCS} ${HEADERS} ${DASM_T} )

install( TARGETS buildvm
        DESTINATION ${BINDIR} )