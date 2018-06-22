cmake_minimum_required(VERSION 2.8)

add_definitions(-DLUA_COMPAT_ALL)

if(LINUX)
    add_definitions(-DLUA_USE_LINUX)
endif(LINUX)

if(APPLE AND NOT IOS)
    add_definitions(-DLUA_USE_MACOSX)
endif(APPLE AND NOT IOS)

set(LUA_PUBLIC_HEADERS lua.h luaconf.h lualib.h lauxlib.h lua.hpp)

set(LUA_SRC lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c
lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c
lundump.c lvm.c lzio.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c
liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c loadlib.c linit.c)

add_library(lua ${LUA_SRC})

install(FILES ${LUA_PUBLIC_HEADERS} DESTINATION include/lua)
