set(TARGET x86_64)

set(MIR_HEADERS 
    mir/mir.h
    mir/mir-gen.h
    mir/mir-varr.h
    mir/mir-dlist.h 
    mir/mir-htab.h 
    mir/mir-hash.h
    mir/mir-bitmap.h
    )

set(MIR_SRCS 
    mir/mir.c
    mir/mir-gen.c
    )

set(C2MIR_SRCS
    mir/c2mir/c2mir.c
    )

set_property(SOURCE ${MIR_SRCS} ${C2MIR_SRCS}
        APPEND
        PROPERTY COMPILE_DEFINITIONS "${TARGET};MIR_IO;MIR_SCAN"
        #PROPERTY INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/mir;${CMAKE_SOURCE_DIR}/mir/c2mir"
        )
