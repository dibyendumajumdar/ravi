# Makefile for building Ravi based on Lua Makefile
# This Makefile builds an interpreter only version right now - no JIT

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= none

#CC= gcc -std=gnu99
CFLAGS= -O2 -Wall -Wextra -DNDEBUG -fomit-frame-pointer -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1 $(SYSCFLAGS) $(MYCFLAGS) -I../include -I../dmr_c/src -I../dmr_c/null-backend
CXXFLAGS=$(CFLAGS) -fno-rtti -Wno-sign-compare -std=c++14 -fno-exceptions -I../include
LDFLAGS= $(SYSLDFLAGS) $(MYLDFLAGS)
LIBS= -lm $(SYSLIBS) $(MYLIBS)
VPATH=../include:../dmr_c/src

AR= ar rcu
RANLIB= ranlib
RM= rm -f

SYSCFLAGS=
SYSLDFLAGS=
SYSLIBS=

MYCFLAGS=
MYLDFLAGS=
MYLIBS=
MYOBJS=

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

PLATS= aix bsd c89 freebsd generic linux macosx mingw posix solaris

LUA_A=	libravinojit.a
CORE_CPP_O=ravijit.o
CORE_O=	lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o \
	lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltests.o \
	ltm.o lundump.o lvm.o lzio.o ravi_profile.o ravi_membuf.o \
	ravi_jitshared.o ravi_nojit.o ravi_ast.o ravi_alloc.o $(CORE_CPP_O)
LIB_O=	lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o \
	lmathlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o loadlib.o linit.o \
	bit.o
DMRC_O= allocate.o builtin.o char.o expression.o evaluate.o expand.o inline.o \
	lib.o linearize.o liveness.o parse.o target.o tokenize.o pre-process.o \
	ptrlist.o scope.o show-parse.o symbol.o walksymbol.o \
	ravi_dmrc_parsesymbols.o 
BASE_O= $(CORE_O) $(LIB_O) $(DMRC_O) $(MYOBJS)

LUA_T=	ravi
LUA_O=	lua.o

ALL_O= $(BASE_O) $(LUA_O) 
ALL_T= $(LUA_A) $(LUA_T) 
ALL_A= $(LUA_A)

# Targets start here.
default: $(PLAT)

all:	$(ALL_T)

o:	$(ALL_O)

a:	$(ALL_A)

$(LUA_A): $(BASE_O)
	$(AR) $@ $(BASE_O)
	$(RANLIB) $@

$(LUA_T): $(LUA_O) $(LUA_A)
	$(CC) -o $@ $(LDFLAGS) $(LUA_O) $(LUA_A) $(LIBS)

clean:
	$(RM) $(ALL_T) $(ALL_O)

depend:
	@$(CC) $(CFLAGS) -MM *.c
	@$(CC) $(CXXFLAGS) -MM ravijit.cpp

echo:
	@echo "PLAT= $(PLAT)"
	@echo "CC= $(CC)"
	@echo "CFLAGS= $(CFLAGS)"
	@echo "LDFLAGS= $(SYSLDFLAGS)"
	@echo "LIBS= $(LIBS)"
	@echo "AR= $(AR)"
	@echo "RANLIB= $(RANLIB)"
	@echo "RM= $(RM)"

# Convenience targets for popular platforms
ALL= all

none:
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"

aix:
	$(MAKE) $(ALL) CC="xlc" CFLAGS="-O2 -DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-ldl" SYSLDFLAGS="-brtl -bexpall"

bsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-Wl,-E"

c89:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_C89" CC="gcc -std=c89"
	@echo ''
	@echo '*** C89 does not guarantee 64-bit integers for Lua.'
	@echo ''


freebsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX" SYSLIBS="-Wl,-E -lreadline"

generic: $(ALL)

linux:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX" SYSLIBS="-Wl,-E -ldl -lreadline"

macosx:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_MACOSX" SYSLIBS="-lreadline" CC=cc

mingw:
	$(MAKE) "LUA_A=ravi.dll" "LUA_T=ravi.exe" \
	"AR=$(CC) -shared -o" "RANLIB=strip --strip-unneeded" \
	"SYSCFLAGS=-DLUA_BUILD_AS_DLL" "SYSLIBS=" "SYSLDFLAGS=-s" ravi.exe

posix:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX"

solaris:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN -D_REENTRANT" SYSLIBS="-ldl"

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) default o a clean depend echo none

allocate.o: allocate.c ../dmr_c/src/allocate.h
ptrlist.o: ptrlist.c ../dmr_c/src/ptrlist.h

# DO NOT DELETE

bit.o: bit.c ../include/lua.h ../include/luaconf.h ../include/lauxlib.h \
 ../include/lua.h
lapi.o: lapi.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/lapi.h ../include/llimits.h ../include/lua.h \
 ../include/lstate.h ../include/lobject.h ../include/ltm.h \
 ../include/lzio.h ../include/lmem.h ../include/ldebug.h ../include/ldo.h \
 ../include/lfunc.h ../include/lgc.h ../include/lmem.h \
 ../include/lobject.h ../include/lstate.h ../include/lstring.h \
 ../include/lgc.h ../include/ltable.h ../include/ltm.h \
 ../include/lundump.h ../include/lvm.h ../include/ldo.h
lauxlib.o: lauxlib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h
lbaselib.o: lbaselib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lbitlib.o: lbitlib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lcode.o: lcode.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lcode.h ../include/llex.h \
 ../include/lobject.h ../include/llimits.h ../include/lua.h \
 ../include/lzio.h ../include/lmem.h ../include/lopcodes.h \
 ../include/lparser.h ../include/ldebug.h ../include/lstate.h \
 ../include/ltm.h ../include/ldo.h ../include/lgc.h ../include/llex.h \
 ../include/lmem.h ../include/lobject.h ../include/lopcodes.h \
 ../include/lparser.h ../include/lstring.h ../include/lgc.h \
 ../include/ltable.h ../include/lvm.h ../include/ldo.h
lcorolib.o: lcorolib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lctype.o: lctype.c ../include/lprefix.h ../include/lctype.h \
 ../include/lua.h ../include/luaconf.h ../include/llimits.h
ldblib.o: ldblib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
ldebug.o: ldebug.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lapi.h ../include/llimits.h \
 ../include/lua.h ../include/lstate.h ../include/lobject.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h ../include/lcode.h \
 ../include/llex.h ../include/lopcodes.h ../include/lparser.h \
 ../include/ldebug.h ../include/ldo.h ../include/lfunc.h \
 ../include/lobject.h ../include/lopcodes.h ../include/lstate.h \
 ../include/lstring.h ../include/lgc.h ../include/ltable.h \
 ../include/ltm.h ../include/lvm.h ../include/ldo.h
ldo.o: ldo.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/lapi.h ../include/llimits.h ../include/lua.h \
 ../include/lstate.h ../include/lobject.h ../include/ltm.h \
 ../include/lzio.h ../include/lmem.h ../include/ldebug.h ../include/ldo.h \
 ../include/lfunc.h ../include/lgc.h ../include/lmem.h \
 ../include/lobject.h ../include/lopcodes.h ../include/lparser.h \
 ../include/lstate.h ../include/lstring.h ../include/lgc.h \
 ../include/ltable.h ../include/ltm.h ../include/lundump.h \
 ../include/lvm.h ../include/ldo.h ../include/lzio.h ../include/ravijit.h \
 ../include/ravi_jitshared.h ../include/lprefix.h ../include/lauxlib.h \
 ../include/ldebug.h ../include/lfunc.h ../include/lopcodes.h \
 ../include/lstring.h ../include/ltable.h ../include/lvm.h \
 ../include/ravi_membuf.h
ldump.o: ldump.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lobject.h ../include/llimits.h \
 ../include/lua.h ../include/lstate.h ../include/lobject.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h \
 ../include/lundump.h
lfunc.o: lfunc.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lfunc.h ../include/lobject.h \
 ../include/llimits.h ../include/lua.h ../include/lgc.h \
 ../include/lstate.h ../include/ltm.h ../include/lzio.h ../include/lmem.h \
 ../include/lmem.h ../include/lobject.h ../include/lstate.h \
 ../include/ravijit.h
lgc.o: lgc.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/ldebug.h ../include/lstate.h ../include/lua.h \
 ../include/lobject.h ../include/llimits.h ../include/ltm.h \
 ../include/lzio.h ../include/lmem.h ../include/ldo.h ../include/lfunc.h \
 ../include/lgc.h ../include/lmem.h ../include/lobject.h \
 ../include/lstate.h ../include/lstring.h ../include/lgc.h \
 ../include/ltable.h ../include/ltm.h
linit.o: linit.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lualib.h ../include/lua.h \
 ../include/lauxlib.h
liolib.o: liolib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
llex.o: llex.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/lctype.h ../include/lua.h ../include/llimits.h \
 ../include/ldebug.h ../include/lstate.h ../include/lobject.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h ../include/ldo.h \
 ../include/lgc.h ../include/llex.h ../include/lobject.h \
 ../include/lparser.h ../include/lstate.h ../include/lstring.h \
 ../include/lgc.h ../include/ltable.h ../include/lzio.h
lmathlib.o: lmathlib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lmem.o: lmem.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/ldebug.h ../include/lstate.h ../include/lua.h \
 ../include/lobject.h ../include/llimits.h ../include/ltm.h \
 ../include/lzio.h ../include/lmem.h ../include/ldo.h ../include/lgc.h \
 ../include/lmem.h ../include/lobject.h ../include/lstate.h
loadlib.o: loadlib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lobject.o: lobject.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lctype.h ../include/lua.h \
 ../include/llimits.h ../include/ldebug.h ../include/lstate.h \
 ../include/lobject.h ../include/ltm.h ../include/lzio.h \
 ../include/lmem.h ../include/ldo.h ../include/lmem.h \
 ../include/lobject.h ../include/lstate.h ../include/lstring.h \
 ../include/lgc.h ../include/lvm.h ../include/ldo.h
lopcodes.o: lopcodes.c ../include/lprefix.h ../include/lopcodes.h \
 ../include/llimits.h ../include/lua.h ../include/luaconf.h \
 ../include/lobject.h ../include/lstate.h ../include/lobject.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h ../include/ldebug.h \
 ../include/lstate.h
loslib.o: loslib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lparser.o: lparser.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lcode.h ../include/llex.h \
 ../include/lobject.h ../include/llimits.h ../include/lua.h \
 ../include/lzio.h ../include/lmem.h ../include/lopcodes.h \
 ../include/lparser.h ../include/ldebug.h ../include/lstate.h \
 ../include/ltm.h ../include/ldo.h ../include/lfunc.h ../include/llex.h \
 ../include/lmem.h ../include/lobject.h ../include/lopcodes.h \
 ../include/lparser.h ../include/lstate.h ../include/lstring.h \
 ../include/lgc.h ../include/ltable.h
lstate.o: lstate.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lapi.h ../include/llimits.h \
 ../include/lua.h ../include/lstate.h ../include/lobject.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h ../include/ldebug.h \
 ../include/ldo.h ../include/lfunc.h ../include/lgc.h ../include/llex.h \
 ../include/lmem.h ../include/lstate.h ../include/lstring.h \
 ../include/lgc.h ../include/ltable.h ../include/ltm.h \
 ../include/ravijit.h ../include/ravi_profile.h ../include/lopcodes.h
lstring.o: lstring.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/ldebug.h ../include/lstate.h \
 ../include/lua.h ../include/lobject.h ../include/llimits.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h ../include/ldo.h \
 ../include/lmem.h ../include/lobject.h ../include/lstate.h \
 ../include/lstring.h ../include/lgc.h
lstrlib.o: lstrlib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
ltable.o: ltable.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/ldebug.h ../include/lstate.h \
 ../include/lua.h ../include/lobject.h ../include/llimits.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h ../include/ldo.h \
 ../include/lgc.h ../include/lmem.h ../include/lobject.h \
 ../include/lstate.h ../include/lstring.h ../include/lgc.h \
 ../include/ltable.h ../include/lvm.h ../include/ldo.h
ltablib.o: ltablib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
ltests.o: ltests.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lapi.h ../include/llimits.h \
 ../include/lua.h ../include/lstate.h ../include/lobject.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h \
 ../include/lauxlib.h ../include/lcode.h ../include/llex.h \
 ../include/lopcodes.h ../include/lparser.h ../include/lctype.h \
 ../include/ldebug.h ../include/ldo.h ../include/lfunc.h \
 ../include/lmem.h ../include/lopcodes.h ../include/lstate.h \
 ../include/lstring.h ../include/lgc.h ../include/ltable.h \
 ../include/lualib.h
ltm.o: ltm.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/ldebug.h ../include/lstate.h ../include/lua.h \
 ../include/lobject.h ../include/llimits.h ../include/ltm.h \
 ../include/lzio.h ../include/lmem.h ../include/ldo.h \
 ../include/lobject.h ../include/lstate.h ../include/lstring.h \
 ../include/lgc.h ../include/ltable.h ../include/ltm.h ../include/lvm.h \
 ../include/ldo.h
lua.o: lua.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/lauxlib.h ../include/lua.h ../include/lualib.h
luac.o: luac.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/lauxlib.h ../include/lua.h ../include/lobject.h \
 ../include/llimits.h ../include/lstate.h ../include/lobject.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h \
 ../include/lundump.h ../include/ldebug.h ../include/lstate.h \
 ../include/lopcodes.h
lundump.o: lundump.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/ldebug.h ../include/lstate.h \
 ../include/lua.h ../include/lobject.h ../include/llimits.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h ../include/ldo.h \
 ../include/lfunc.h ../include/lmem.h ../include/lobject.h \
 ../include/lstring.h ../include/lgc.h ../include/lundump.h \
 ../include/lzio.h
lutf8lib.o: lutf8lib.c ../include/lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lvm.o: lvm.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/ldebug.h ../include/lstate.h ../include/lua.h \
 ../include/lobject.h ../include/llimits.h ../include/ltm.h \
 ../include/lzio.h ../include/lmem.h ../include/ldo.h ../include/lfunc.h \
 ../include/lgc.h ../include/lobject.h ../include/lopcodes.h \
 ../include/lstate.h ../include/lstring.h ../include/lgc.h \
 ../include/ltable.h ../include/ltm.h ../include/lvm.h ../include/ldo.h \
 ../include/ravi_profile.h ../include/lopcodes.h
lzio.o: lzio.c ../include/lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/llimits.h ../include/lua.h ../include/lmem.h \
 ../include/llimits.h ../include/lstate.h ../include/lobject.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h ../include/lzio.h
ravi_jitshared.o: ravi_jitshared.c ../include/ravi_jitshared.h \
 ../include/lprefix.h ../include/lauxlib.h ../include/lua.h \
 ../include/luaconf.h ../include/ldebug.h ../include/lstate.h \
 ../include/lobject.h ../include/llimits.h ../include/ltm.h \
 ../include/lzio.h ../include/lmem.h ../include/ldo.h ../include/lfunc.h \
 ../include/lopcodes.h ../include/lstring.h ../include/lgc.h \
 ../include/ltable.h ../include/lvm.h ../include/ravi_membuf.h
ravi_membuf.o: ravi_membuf.c ../include/ravi_membuf.h
ravi_nojit.o: ravi_nojit.c ../include/ravijit.h \
 ../include/ravi_jitshared.h ../include/lprefix.h ../include/lauxlib.h \
 ../include/lua.h ../include/luaconf.h ../include/ldebug.h \
 ../include/lstate.h ../include/lobject.h ../include/llimits.h \
 ../include/ltm.h ../include/lzio.h ../include/lmem.h ../include/ldo.h \
 ../include/lfunc.h ../include/lopcodes.h ../include/lstring.h \
 ../include/lgc.h ../include/ltable.h ../include/lvm.h \
 ../include/ravi_membuf.h
ravi_profile.o: ravi_profile.c ../include/ravi_profile.h ../include/lua.h \
 ../include/luaconf.h ../include/lopcodes.h ../include/llimits.h \
 ../include/lstate.h ../include/lobject.h ../include/ltm.h \
 ../include/lzio.h ../include/lmem.h
ravijit.o: ravijit.cpp ../include/ravijit.h ../include/ravi_jitshared.h \
 ../include/lprefix.h ../include/lauxlib.h ../include/lua.h \
 ../include/luaconf.h ../include/ldebug.h ../include/lstate.h \
 ../include/lobject.h ../include/llimits.h ../include/ltm.h \
 ../include/lzio.h ../include/lmem.h ../include/ldo.h ../include/lfunc.h \
 ../include/lopcodes.h ../include/lstring.h ../include/lgc.h \
 ../include/ltable.h ../include/lvm.h ../include/ravi_membuf.h \
 ../include/lapi.h ../include/lauxlib.h ../include/lfunc.h \
 ../include/lobject.h ../include/lopcodes.h ../include/lstate.h \
 ../include/lua.h
