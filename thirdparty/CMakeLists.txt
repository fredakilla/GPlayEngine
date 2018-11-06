cmake_minimum_required(VERSION 3.1)
project(gplay-thirdparty)

include(../global.cmake)


# set cmake default output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
if(WINDOWS)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
endif()

# set output directories
set(OUT_DIR ${CMAKE_SOURCE_DIR}/../output)
set(OUT_DIR_INCLUDE ${OUT_DIR}/include/gplayengine/thirdparty)
set(OUT_DIR_LIB ${OUT_DIR}/lib)

IF(NOT WINDOWS)
    # For Windows, let the user pick since we need to generate both 
    # Debug and Release variants of all the libs.
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT WINDOWS)

set(CMAKE_DEBUG_POSTFIX "")

# create output directory
file(MAKE_DIRECTORY ${OUT_DIR_LIB})


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
set( USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "Use MSVC Runtime Library DLL (/MD or /MDd)" FORCE)
add_subdirectory(bullet)

if(LINUX)
    # openal as static
    set(LIBTYPE STATIC)
endif()

if(LINUX OR WINDOWS)
    # SDL2
    set( SDL_SHARED_ENABLED_BY_DEFAULT OFF )
    add_subdirectory(SDL2)
    add_subdirectory(openal)
    #add_subdirectory(freetype)
    add_subdirectory(libpng)        # only needed by gplay-encoder
endif()

# add subdirectory
add_subdirectory(splines)
add_subdirectory(libjson)
add_subdirectory(tinyxml2)
add_subdirectory(lua)
#add_subdirectory(openal)
add_subdirectory(zlib)
add_subdirectory(ogg)       # Ogg needs to be built before vorbis
add_subdirectory(vorbis)
add_subdirectory(base64)
# include(${PROJECT_SOURCE_DIR}/cmake/imgui.cmake)	# imgui is now provided by bgfx 3rdparty
add_subdirectory(spark)
add_subdirectory(efsw)
# add_subdirectory(freetype)		# freetype is now provided by bgfx 3rdparty

set(BUILD_TARGETS
    BulletDynamics
    LinearMath
    BulletCollision
    vorbisfile
    vorbisenc
    vorbis
    ogg
    zlibstatic
    lua
    tinyxml2static
    json
    base64
    bgfx
    bimg
    bx
    brtshaderc
    bgfxcommon
    # imgui
    spark
    efsw
)

# Emscripten ships with its own implementation of the OpenAL, sdl2, freetype
if(NOT EMSCRIPTEN)
    list(APPEND BUILD_TARGETS SDL2-static)
    list(APPEND BUILD_TARGETS SDL2main)
    # list(APPEND BUILD_TARGETS freetype)
    list(APPEND BUILD_TARGETS OpenAL)
    list(APPEND BUILD_TARGETS png_static)
endif()



# amalgamate libraries

if(LINUX)
    set(GLAY_DEPS_LIB libgplay-deps.a)
    set(TARGET_CMD ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gnu-amalgamate.sh
        ${CMAKE_AR}
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${OUT_DIR_LIB}
    )
elseif(EMSCRIPTEN)
    set(GLAY_DEPS_LIB libgplay-deps.a)
    set(TARGET_CMD ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gnu-amalgamate.sh
        $(EMSCRIPTEN)/emar
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${OUT_DIR_LIB}
    )
elseif(WINDOWS)
    set(GLAY_DEPS_LIB gplay-deps.lib)
    set(TARGET_CMD ${CMAKE_CURRENT_SOURCE_DIR}/cmake/msvc-amalgamate.bat
        ${CMAKE_AR}
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${OUT_DIR_LIB}
    )
endif()

add_custom_command(
    OUTPUT ${GLAY_DEPS_LIB}
    COMMAND ${TARGET_CMD}
    #DEPENDS ${BUILD_TARGETS}
    COMMENT "Amalgamate external dependencies..."
)

add_custom_target(GPLAY_DEPS ALL DEPENDS ${GLAY_DEPS_LIB})
add_dependencies(GPLAY_DEPS ${BUILD_TARGETS})

# copy thirdparty headers to output directory

COPY_HEADERS(base64/base64.h ${OUT_DIR_INCLUDE}/base64/)
COPY_HEADERS(bullet/src/ ${OUT_DIR_INCLUDE}/)
COPY_HEADERS(efsw/include/ ${OUT_DIR_INCLUDE}/)
COPY_HEADERS(libjson/ ${OUT_DIR_INCLUDE}/libjson/)
COPY_HEADERS(lua/src/ ${OUT_DIR_INCLUDE}/lua/)
COPY_HEADERS(ogg/include/ogg/ ${OUT_DIR_INCLUDE}/ogg/)
COPY_HEADERS(openal/include/AL/ ${OUT_DIR_INCLUDE}/AL/)
COPY_HEADERS(SDL2/include/ ${OUT_DIR_INCLUDE}/SDL2/)
COPY_HEADERS(spark/spark/include/ ${OUT_DIR_INCLUDE}/spark/)
COPY_HEADERS(tinyxml2/tinyxml2.h ${OUT_DIR_INCLUDE}/tinyxml2/)
COPY_HEADERS(vorbis/include/vorbis/ ${OUT_DIR_INCLUDE}/vorbis/)
COPY_HEADERS(zlib/zlib.h ${OUT_DIR_INCLUDE}/zlib/)
COPY_HEADERS(libpng/ ${OUT_DIR_INCLUDE}/png/)
COPY_HEADERS(freetype/include/freetype/ ${OUT_DIR_INCLUDE}/freetype/)
COPY_HEADERS(freetype/include/ft2build.h ${OUT_DIR_INCLUDE}/)
#COPY_HEADERS(assimp/include/ ${OUT_DIR_INCLUDE}/)
COPY_HEADERS(splines/src/Splines.hh ${OUT_DIR_INCLUDE}/splines/)

# copy thirdparty bgfx stuff to output directory

COPY_HEADERS(bgfx-cmake/bgfx/include/ ${OUT_DIR_INCLUDE}/)
COPY_HEADERS(bgfx-cmake/bimg/include/ ${OUT_DIR_INCLUDE}/)
COPY_HEADERS(bgfx-cmake/bx/include/ ${OUT_DIR_INCLUDE}/)
COPY_HEADERS(bgfx-cmake/brtshaderc/brtshaderc.h ${OUT_DIR_INCLUDE}/brtshaderc/)
COPY_HEADERS(bgfx-cmake/bgfx/3rdparty/dear-imgui/ ${OUT_DIR_INCLUDE}/dear-imgui/)
COPY_HEADERS(bgfx-cmake/bgfx/3rdparty/iconfontheaders/ ${OUT_DIR_INCLUDE}/iconfontheaders/)
COPY_HEADERS(bgfx-cmake/bgfxcommon/ ${OUT_DIR_INCLUDE}/bgfxcommon/)
COPY_HEADERS(bgfx-cmake/bgfx/3rdparty/stb/ ${OUT_DIR_INCLUDE}/stb/)

if(WINDOWS)
    COPY_HEADERS(bgfx-cmake/bx/include/compat/ ${OUT_DIR_INCLUDE}/bx/compat/)
endif()



# pre-steps before building engine

# copy engine headers to output directory
COPY_HEADERS(${CMAKE_CURRENT_SOURCE_DIR}/../src/ ${OUT_DIR}/include/gplayengine/)

# write setup.pri file for Qt
get_filename_component(result "${OUT_DIR}" ABSOLUTE)
file(WRITE ../setup.pri "GPLAY_OUTPUT_DIR=${result}")

# copy res directory to output bin directory (or create symbolic link on supported systems)
file(MAKE_DIRECTORY ${OUT_DIR}/bin)
if(WINDOWS)
    # add copy command
elseif(LINUX)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/../res ${OUT_DIR}/bin/res)
endif()


