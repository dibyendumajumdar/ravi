# Makefile for building Ravi based on Lua Makefile
# This Makefile builds an interpreter only version right now - no JIT

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= guess

CC= gcc -std=gnu99
# Note for Ravi we need NDEBUG to disable LTESTS
# Also we enable some GCC specific flags in lvm.c via RAVI_ENABLE_GCC_FLAGS
CFLAGS= -O2 -DNDEBUG -DRAVI_ENABLE_GCC_FLAGS -Wall $(SYSCFLAGS) $(MYCFLAGS) -I../include
LDFLAGS= $(SYSLDFLAGS) $(MYLDFLAGS)
LIBS= -lm $(SYSLIBS) $(MYLIBS)
VPATH=../include

AR= ar rcu
RANLIB= ranlib
RM= rm -f
UNAME= uname

SYSCFLAGS=
SYSLDFLAGS=
SYSLIBS=

MYCFLAGS=
MYLDFLAGS=
MYLIBS=
MYOBJS=

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

PLATS= guess aix bsd c89 freebsd generic linux linux-readline macosx mingw posix solaris

LUA_A=	libravinojit.a
CORE_O=	lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o \
	lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltests.o \
	ltm.o lundump.o lvm.o lzio.o ravi_profile.o ravi_membuf.o \
	ravi_jitshared.o ravi_nojit.o ravi_alloc.o ravi_jit.o
LIB_O=	lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o \
	lmathlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o loadlib.o linit.o \
	bit.o
BASE_O= $(CORE_O) $(LIB_O) $(MYOBJS)

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

echo:
	@echo "PLAT= $(PLAT)"
	@echo "CC= $(CC)"
	@echo "CFLAGS= $(CFLAGS)"
	@echo "LDFLAGS= $(SYSLDFLAGS)"
	@echo "LIBS= $(LIBS)"
	@echo "AR= $(AR)"
	@echo "RANLIB= $(RANLIB)"
	@echo "RM= $(RM)"
	@echo "UNAME= $(UNAME)"

# Convenience targets for popular platforms
ALL= all

help:
	@echo "Do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"

guess:
	@echo Guessing `$(UNAME)`
	@$(MAKE) `$(UNAME)`

AIX aix:
	$(MAKE) $(ALL) CC="xlc" CFLAGS="-O2 -DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-ldl" SYSLDFLAGS="-brtl -bexpall"

bsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-Wl,-E"

c89:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_C89" CC="gcc -std=c89"
	@echo ''
	@echo '*** C89 does not guarantee 64-bit integers for Lua.'
	@echo ''

FreeBSD NetBSD OpenBSD freebsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX -DLUA_USE_READLINE -I/usr/include/edit" SYSLIBS="-Wl,-E -ledit" CC="cc"

generic: $(ALL)

Linux linux:	linux-readline

linux-noreadline:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX" SYSLIBS="-Wl,-E -ldl"

linux-readline:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX -DLUA_USE_READLINE" SYSLIBS="-Wl,-E -ldl -lreadline"

Darwin macos macosx:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_MACOSX -DLUA_USE_READLINE" SYSLIBS="-lreadline"

mingw:
	$(MAKE) "LUA_A=ravi.dll" "LUA_T=ravi.exe" \
	"AR=$(CC) -shared -o" "RANLIB=strip --strip-unneeded" \
	"SYSCFLAGS=-DLUA_BUILD_AS_DLL" "SYSLIBS=" "SYSLDFLAGS=-s" ravi.exe

posix:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX"

SunOS solaris:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN -D_REENTRANT" SYSLIBS="-ldl"

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) default o a clean depend echo none

# DO NOT DELETE

bit.o: bit.c ../include/lua.h ../include/luaconf.h ../include/lauxlib.h \
 ../include/lua.h
lapi.o: lapi.c lprefix.h ../include/lua.h ../include/luaconf.h \
 lapi.h llimits.h ../include/lua.h \
 lstate.h lobject.h ltm.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h ldebug.h ldo.h \
 lfunc.h lgc.h lmem.h \
 lobject.h lstate.h lstring.h \
 lgc.h ltable.h ltm.h \
 lundump.h lvm.h ldo.h
lauxlib.o: lauxlib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ravi_alloc.h
lbaselib.o: lbaselib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lbitlib.o: lbitlib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lcode.o: lcode.c lprefix.h ../include/lua.h \
 ../include/luaconf.h lcode.h llex.h \
 lobject.h llimits.h ../include/lua.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h lparser.h ldebug.h \
 lstate.h ltm.h ldo.h lgc.h \
 llex.h lmem.h lobject.h \
 lopcodes.h lparser.h lstring.h \
 lgc.h ltable.h lvm.h ldo.h
lcorolib.o: lcorolib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lctype.o: lctype.c lprefix.h lctype.h \
 ../include/lua.h ../include/luaconf.h llimits.h
ldblib.o: ldblib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
ldebug.o: ldebug.c lprefix.h ../include/lua.h \
 ../include/luaconf.h lapi.h llimits.h \
 ../include/lua.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h lcode.h \
 llex.h lparser.h ldebug.h \
 ldo.h lfunc.h lobject.h \
 lopcodes.h lstate.h lstring.h \
 lgc.h ltable.h ltm.h lvm.h \
 ldo.h
ldo.o: ldo.c lprefix.h ../include/lua.h ../include/luaconf.h \
 lapi.h llimits.h ../include/lua.h \
 lstate.h lobject.h ltm.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h ldebug.h ldo.h \
 lfunc.h lgc.h lmem.h \
 lobject.h lopcodes.h lparser.h \
 lstate.h lstring.h lgc.h \
 ltable.h ltm.h lundump.h \
 lvm.h ldo.h lzio.h ravi_jit.h \
 ravi_jitshared.h lprefix.h ../include/lauxlib.h \
 ldebug.h lfunc.h lstring.h \
 ltable.h lvm.h ravi_membuf.h
ldump.o: ldump.c lprefix.h ../include/lua.h \
 ../include/luaconf.h lobject.h llimits.h \
 ../include/lua.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h lundump.h
lfunc.o: lfunc.c lprefix.h ../include/lua.h \
 ../include/luaconf.h lfunc.h lobject.h \
 llimits.h ../include/lua.h lgc.h \
 lstate.h ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h lmem.h \
 lobject.h lstate.h ravi_jit.h
lgc.o: lgc.c lprefix.h ../include/lua.h ../include/luaconf.h \
 ldebug.h lstate.h ../include/lua.h \
 lobject.h llimits.h ltm.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h ldo.h lfunc.h \
 lgc.h lmem.h lobject.h \
 lstate.h lstring.h lgc.h \
 ltable.h ltm.h
linit.o: linit.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lualib.h ../include/lua.h \
 ../include/lauxlib.h
liolib.o: liolib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
llex.o: llex.c lprefix.h ../include/lua.h ../include/luaconf.h \
 lctype.h ../include/lua.h llimits.h \
 ldebug.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h ldo.h \
 lgc.h llex.h lobject.h \
 lparser.h lstate.h lstring.h \
 lgc.h ltable.h lzio.h
lmathlib.o: lmathlib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lmem.o: lmem.c lprefix.h ../include/lua.h ../include/luaconf.h \
 ldebug.h lstate.h ../include/lua.h \
 lobject.h llimits.h ltm.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h ldo.h lgc.h \
 lmem.h lobject.h lstate.h
loadlib.o: loadlib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lobject.o: lobject.c lprefix.h ../include/lua.h \
 ../include/luaconf.h lctype.h ../include/lua.h \
 llimits.h ldebug.h lstate.h \
 lobject.h ltm.h lzio.h \
 lmem.h lopcodes.h ravi_arch.h \
 ldo.h lmem.h lobject.h \
 lstate.h lstring.h lgc.h \
 lvm.h ldo.h
lopcodes.o: lopcodes.c lprefix.h lopcodes.h \
 llimits.h ../include/lua.h ../include/luaconf.h \
 ravi_arch.h lobject.h lstate.h \
 lobject.h ltm.h lzio.h \
 lmem.h lopcodes.h ldebug.h \
 lstate.h
loslib.o: loslib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lparser.o: lparser.c lprefix.h ../include/lua.h \
 ../include/luaconf.h lcode.h llex.h \
 lobject.h llimits.h ../include/lua.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h lparser.h ldebug.h \
 lstate.h ltm.h ldo.h lfunc.h \
 llex.h lmem.h lobject.h \
 lopcodes.h lparser.h lstate.h \
 lstring.h lgc.h ltable.h
lstate.o: lstate.c lprefix.h ../include/lua.h \
 ../include/luaconf.h lapi.h llimits.h \
 ../include/lua.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h ldebug.h \
 ldo.h lfunc.h lgc.h llex.h \
 lmem.h lstate.h lstring.h \
 lgc.h ltable.h ltm.h \
 ravi_jit.h ravi_profile.h ravi_alloc.h
lstring.o: lstring.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ldebug.h lstate.h \
 ../include/lua.h lobject.h llimits.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h ldo.h \
 lmem.h lobject.h lstate.h \
 lstring.h lgc.h
lstrlib.o: lstrlib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
ltable.o: ltable.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ldebug.h lstate.h \
 ../include/lua.h lobject.h llimits.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h ldo.h \
 lgc.h lmem.h lobject.h \
 lstate.h lstring.h lgc.h \
 ltable.h lvm.h ldo.h
ltablib.o: ltablib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
ltests.o: ltests.c lprefix.h ../include/lua.h \
 ../include/luaconf.h lapi.h llimits.h \
 ../include/lua.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h ../include/lauxlib.h \
 lcode.h llex.h lparser.h \
 lctype.h ldebug.h ldo.h \
 lfunc.h lmem.h lopcodes.h \
 lstate.h lstring.h lgc.h \
 ltable.h ../include/lualib.h
ltm.o: ltm.c lprefix.h ../include/lua.h ../include/luaconf.h \
 ldebug.h lstate.h ../include/lua.h \
 lobject.h llimits.h ltm.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h ldo.h lobject.h \
 lstate.h lstring.h lgc.h \
 ltable.h ltm.h lvm.h ldo.h
lua.o: lua.c lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/lauxlib.h ../include/lua.h ../include/lualib.h
luac.o: luac.c lprefix.h ../include/lua.h ../include/luaconf.h \
 ../include/lauxlib.h ../include/lua.h lobject.h \
 llimits.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h lundump.h \
 ldebug.h lstate.h lopcodes.h
lundump.o: lundump.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ldebug.h lstate.h \
 ../include/lua.h lobject.h llimits.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h ldo.h \
 lfunc.h lmem.h lobject.h \
 lstring.h lgc.h lundump.h \
 lzio.h
lutf8lib.o: lutf8lib.c lprefix.h ../include/lua.h \
 ../include/luaconf.h ../include/lauxlib.h ../include/lua.h \
 ../include/lualib.h
lvm.o: lvm.c lprefix.h ../include/lua.h ../include/luaconf.h \
 ldebug.h lstate.h ../include/lua.h \
 lobject.h llimits.h ltm.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h ldo.h lfunc.h \
 lgc.h lobject.h lopcodes.h \
 lstate.h lstring.h lgc.h \
 ltable.h ltm.h lvm.h ldo.h \
 ravi_profile.h
lzio.o: lzio.c lprefix.h ../include/lua.h ../include/luaconf.h \
 llimits.h ../include/lua.h lmem.h \
 llimits.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h lzio.h
ravi_alloc.o: ravi_alloc.c
ravi_jitshared.o: ravi_jitshared.c ravi_jitshared.h \
 lprefix.h ../include/lauxlib.h ../include/lua.h \
 ../include/luaconf.h ldebug.h lstate.h \
 lobject.h llimits.h ltm.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h ldo.h lfunc.h \
 lstring.h lgc.h ltable.h \
 lvm.h ravi_membuf.h
ravi_membuf.o: ravi_membuf.c ravi_membuf.h
ravi_nojit.o: ravi_nojit.c ravi_jit.h \
 ravi_jitshared.h lprefix.h ../include/lauxlib.h \
 ../include/lua.h ../include/luaconf.h ldebug.h \
 lstate.h lobject.h llimits.h \
 ltm.h lzio.h lmem.h \
 lopcodes.h ravi_arch.h ldo.h \
 lfunc.h lstring.h lgc.h \
 ltable.h lvm.h ravi_membuf.h
ravi_profile.o: ravi_profile.c ravi_profile.h ../include/lua.h \
 ../include/luaconf.h lopcodes.h llimits.h \
 ravi_arch.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h
ravi_jit.o: ravi_jit.c ravi_jit.h ravi_jitshared.h \
 lprefix.h ../include/lauxlib.h ../include/lua.h \
 ../include/luaconf.h ldebug.h lstate.h \
 lobject.h llimits.h ltm.h \
 lzio.h lmem.h lopcodes.h \
 ravi_arch.h ldo.h lfunc.h \
 lstring.h lgc.h ltable.h \
 lvm.h ravi_membuf.h lapi.h \
 ../include/lauxlib.h lfunc.h lobject.h \
 lopcodes.h lstate.h ../include/lua.h
