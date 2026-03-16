cmake_minimum_required(VERSION 3.15)
project(lua LANGUAGES C)

# Lua kaynak klasörü
set(LUA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Lua kaynak dosyaları (tüm standart modüller dahil)
set(LUA_SOURCES
        ${LUA_SRC_DIR}/lapi.c
        ${LUA_SRC_DIR}/lcode.c
        ${LUA_SRC_DIR}/lctype.c
        ${LUA_SRC_DIR}/ldebug.c
        ${LUA_SRC_DIR}/ldo.c
        ${LUA_SRC_DIR}/ldump.c
        ${LUA_SRC_DIR}/lfunc.c
        ${LUA_SRC_DIR}/lgc.c
        ${LUA_SRC_DIR}/llex.c
        ${LUA_SRC_DIR}/lmem.c
        ${LUA_SRC_DIR}/lobject.c
        ${LUA_SRC_DIR}/lopcodes.c
        ${LUA_SRC_DIR}/lparser.c
        ${LUA_SRC_DIR}/lstate.c
        ${LUA_SRC_DIR}/lstring.c
        ${LUA_SRC_DIR}/ltable.c
        ${LUA_SRC_DIR}/ltm.c
        ${LUA_SRC_DIR}/lundump.c
        ${LUA_SRC_DIR}/lvm.c
        ${LUA_SRC_DIR}/lzio.c

        # Kütüphaneler
        ${LUA_SRC_DIR}/lauxlib.c
        ${LUA_SRC_DIR}/lbaselib.c
        ${LUA_SRC_DIR}/lcorolib.c
        ${LUA_SRC_DIR}/ldblib.c
        ${LUA_SRC_DIR}/liolib.c
        ${LUA_SRC_DIR}/lmathlib.c
        ${LUA_SRC_DIR}/loslib.c
        ${LUA_SRC_DIR}/lstrlib.c
        ${LUA_SRC_DIR}/ltablib.c
        ${LUA_SRC_DIR}/loadlib.c
        ${LUA_SRC_DIR}/lutf8lib.c
        ${LUA_SRC_DIR}/linit.c
)

# Statik Lua kütüphanesi
add_library(lua STATIC ${LUA_SOURCES})

# Header dosyalarını include et
target_include_directories(lua PUBLIC ${LUA_SRC_DIR})

# Platform özel tanımlar (macOS, Linux)
if(APPLE OR UNIX)
    target_compile_definitions(lua PRIVATE LUA_USE_POSIX LUA_USE_DLOPEN)
endif()