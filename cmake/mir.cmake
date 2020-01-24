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

set_source_files_properties(${MIR_SRCS} PROPERTIES
        COMPILE_DEFINITIONS "${TARGET};MIR_IO;MIR_SCAN")

include_directories("mir")
include_directories("mir/c2mir")
