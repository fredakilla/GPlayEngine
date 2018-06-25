cmake_minimum_required(VERSION 3.1)
project(gplay-thirdparty)

IF(NOT WIN32)
    # For Windows, let the user pick since we need to generate both 
    # Debug and Release variants of all the libs.
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT WIN32)

# bgfx, bx and bimg
set( BGFX_BUILD_TOOLS OFF CACHE BOOL "Build bgfx tools." FORCE )
set( BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Build bgfx examples." FORCE )
set( BGFX_INSTALL OFF CACHE BOOL "Create installation target." FORCE )
set( BGFX_INSTALL_EXAMPLES OFF CACHE BOOL "Install examples and their runtimes." FORCE )
set( BGFX_CUSTOM_TARGETS OFF CACHE BOOL "Include convenience custom targets." FORCE )
set( BGFX_USE_OVR OFF CACHE BOOL "Build with OVR support." FORCE )
set( BGFX_AMALGAMATED OFF CACHE BOOL "Amalgamated bgfx build for faster compilation" FORCE )
set( BX_AMALGAMATED OFF CACHE BOOL "Amalgamated bx build for faster compilation" FORCE )
set( BGFX_CONFIG_DEBUG OFF CACHE BOOL "Enables debug configuration on all builds" FORCE )
add_subdirectory(bgfx-cmake)

# Bullet
set( USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "Use Graphical Benchmark" FORCE )
set( BUILD_SHARED_LIBS OFF CACHE BOOL "Use shared libraries" FORCE )
set( BUILD_CPU_DEMOS OFF CACHE BOOL "Build original Bullet CPU examples" FORCE )
set( BUILD_BULLET3 ON CACHE BOOL "Set when you want to build Bullet 3" FORCE )
set( BUILD_PYBULLET OFF CACHE BOOL "Set when you want to build pybullet (experimental Python bindings for Bullet)" FORCE )
set( BUILD_OPENGL3_DEMOS OFF CACHE BOOL "Set when you want to build the OpenGL3+ demos" FORCE )
set( BUILD_EXTRAS OFF CACHE BOOL "Set when you want to build the extras" FORCE )
set( INSTALL_LIBS OFF CACHE BOOL "Set when you want to install libraries" FORCE )
set( BUILD_UNIT_TESTS OFF CACHE BOOL "Build Unit Tests" FORCE )
set( BUILD_BULLET2_DEMOS OFF CACHE BOOL "Set when you want to build the Bullet 2 demos" FORCE )
add_subdirectory(bullet)

# SDL2
set( SDL_SHARED_ENABLED_BY_DEFAULT OFF )
add_subdirectory(SDL2)

# add subdirectory
add_subdirectory(libjson)
add_subdirectory(tinyxml2)
add_subdirectory(lua)
add_subdirectory(openal)
add_subdirectory(zlib)
add_subdirectory(libpng)
add_subdirectory(ogg)   # Ogg needs to be built before vorbis
add_subdirectory(vorbis)
add_subdirectory(base64)
include(${PROJECT_SOURCE_DIR}/cmake/imgui.cmake)
add_subdirectory(spark)
add_subdirectory(efsw)

set(BUILD_TARGETS
    BulletDynamics
    LinearMath
    BulletCollision
    vorbisfile
    vorbisenc
    vorbis
    ogg
    png_static
    zlibstatic
    lua
    tinyxml2static
    json
    base64
    SDL2-static
    bgfx
    bimg
    bx
    brtshaderc
    imgui
    spark
    efsw
)


OPTION(BUILD_WITH_TOOLS "Build gplay tools"	ON)
if(BUILD_WITH_TOOLS)
    include(${PROJECT_SOURCE_DIR}/cmake/assimp.cmake)
    set(BUILD_TARGETS assimp)
endif()


if(LINUX)
    set(GLAY_DEPS libgplay-deps.a)
elseif(WINDOWS)
    set(GLAY_DEPS gplay-deps.lib)
endif(LINUX)

if(LINUX)
    add_custom_command(
        OUTPUT ${GLAY_DEPS}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gnu-amalgamate.sh ${CMAKE_AR} ${OUT_DIR_LIB}
    )
elseif(WINDOWS)
    # Windows
    add_custom_command(
        OUTPUT ${GLAY_DEPS}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmake/msvc-amalgamate.bat ${OUT_DIR_LIB}/${CMAKE_CFG_INTDIR}
    )
endif(LINUX)

add_custom_target( GPLAY_DEPS_TARGET ALL DEPENDS ${GLAY_DEPS})
add_dependencies( GPLAY_DEPS_TARGET ${BUILD_TARGETS})

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${OUT_DIR_LIB}/${GLAY_DEPS})


# copy headers to build directory
set(OUT_INCLUDE_THIRDPARTY_DIR ${OUT_DIR_INCLUDE}/gplayengine/thirdparty)
COPY_HEADERS(base64/base64.h ${OUT_INCLUDE_THIRDPARTY_DIR}/base64/)
COPY_HEADERS(bgfx-cmake/bgfx/include/ ${OUT_INCLUDE_THIRDPARTY_DIR}/)
COPY_HEADERS(bgfx-cmake/bimg/include/ ${OUT_INCLUDE_THIRDPARTY_DIR}/)
COPY_HEADERS(bgfx-cmake/bx/include/ ${OUT_INCLUDE_THIRDPARTY_DIR}/)
COPY_HEADERS(bgfx-cmake/brtshaderc/brtshaderc.h ${OUT_INCLUDE_THIRDPARTY_DIR}/brtshaderc/)
COPY_HEADERS(bullet/src/ ${OUT_INCLUDE_THIRDPARTY_DIR}/)
COPY_HEADERS(efsw/include/ ${OUT_INCLUDE_THIRDPARTY_DIR}/)
COPY_HEADERS(imgui/imconfig.h ${OUT_INCLUDE_THIRDPARTY_DIR}/imgui/)
COPY_HEADERS(imgui/imgui.h ${OUT_INCLUDE_THIRDPARTY_DIR}/imgui/)
COPY_HEADERS(imgui/imgui_internal.h ${OUT_INCLUDE_THIRDPARTY_DIR}/imgui/)
COPY_HEADERS(libjson/ ${OUT_INCLUDE_THIRDPARTY_DIR}/libjson/)
COPY_HEADERS(lua/src/ ${OUT_INCLUDE_THIRDPARTY_DIR}/lua/)
COPY_HEADERS(ogg/include/ogg/ ${OUT_INCLUDE_THIRDPARTY_DIR}/ogg/)
COPY_HEADERS(openal/include/AL/ ${OUT_INCLUDE_THIRDPARTY_DIR}/AL/)
COPY_HEADERS(SDL2/include/ ${OUT_INCLUDE_THIRDPARTY_DIR}/SDL2/)
COPY_HEADERS(spark/spark/include/ ${OUT_INCLUDE_THIRDPARTY_DIR}/spark/)
COPY_HEADERS(tinyxml2/tinyxml2.h ${OUT_INCLUDE_THIRDPARTY_DIR}/tinyxml2/)
COPY_HEADERS(vorbis/include/vorbis/ ${OUT_INCLUDE_THIRDPARTY_DIR}/vorbis/)
COPY_HEADERS(zlib/zlib.h ${OUT_INCLUDE_THIRDPARTY_DIR}/zlib/)
COPY_HEADERS(libpng/ ${OUT_INCLUDE_THIRDPARTY_DIR}/png/)
COPY_HEADERS(freetype/include/freetype/ ${OUT_INCLUDE_THIRDPARTY_DIR}/freetype/)
COPY_HEADERS(freetype/include/ft2build.h ${OUT_INCLUDE_THIRDPARTY_DIR}/)
COPY_HEADERS(assimp/include/ ${OUT_INCLUDE_THIRDPARTY_DIR}/)
