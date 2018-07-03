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


if(LINUX)
    set(GLAY_DEPS_LIB libgplay-deps.a)
    set(TARGET_CMD ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gnu-amalgamate.sh ${CMAKE_AR} ${OUT_DIR_LIB_DEPS})
elseif(WINDOWS)
    set(GLAY_DEPS_LIB gplay-deps.lib)
    set(TARGET_CMD ${CMAKE_CURRENT_SOURCE_DIR}/cmake/msvc-amalgamate.bat ${CMAKE_AR} ${OUT_DIR_LIB_DEPS})
endif(LINUX)


add_custom_command(
    OUTPUT ${GLAY_DEPS_LIB}
    COMMAND ${TARGET_CMD}
    DEPENDS ${BUILD_TARGETS}
    COMMENT "Amalgamate external dependencies..."
)

add_custom_target(GPLAY_DEPS ALL DEPENDS ${GLAY_DEPS_LIB})



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
#COPY_HEADERS(assimp/include/ ${OUT_INCLUDE_THIRDPARTY_DIR}/)

if(WINDOWS)
    COPY_HEADERS(bgfx-cmake/bx/include/compat/ ${OUT_INCLUDE_THIRDPARTY_DIR}/bx/compat/)
endif()



# Organize folders in visual studio
if(WINDOWS)
set_target_properties (BulletDynamics   PROPERTIES FOLDER Thirdparty)
set_target_properties (LinearMath       PROPERTIES FOLDER Thirdparty)
set_target_properties (BulletCollision  PROPERTIES FOLDER Thirdparty)
set_target_properties (vorbisfile       PROPERTIES FOLDER Thirdparty)
set_target_properties (vorbisenc        PROPERTIES FOLDER Thirdparty)
set_target_properties (vorbis           PROPERTIES FOLDER Thirdparty)
set_target_properties (ogg              PROPERTIES FOLDER Thirdparty)
set_target_properties (png_static       PROPERTIES FOLDER Thirdparty)
set_target_properties (zlibstatic       PROPERTIES FOLDER Thirdparty)
set_target_properties (lua              PROPERTIES FOLDER Thirdparty)
set_target_properties (tinyxml2static   PROPERTIES FOLDER Thirdparty)
set_target_properties (json             PROPERTIES FOLDER Thirdparty)
set_target_properties (base64           PROPERTIES FOLDER Thirdparty)
set_target_properties (SDL2-static      PROPERTIES FOLDER Thirdparty)
set_target_properties (bgfx             PROPERTIES FOLDER Thirdparty)
set_target_properties (bimg             PROPERTIES FOLDER Thirdparty)
set_target_properties (bx               PROPERTIES FOLDER Thirdparty)
set_target_properties (brtshaderc       PROPERTIES FOLDER Thirdparty)
set_target_properties (imgui            PROPERTIES FOLDER Thirdparty)
set_target_properties (spark            PROPERTIES FOLDER Thirdparty)
set_target_properties (efsw             PROPERTIES FOLDER Thirdparty)

set_target_properties (Bullet2FileLoader        PROPERTIES FOLDER Thirdparty)
set_target_properties (Bullet3Collision         PROPERTIES FOLDER Thirdparty)
set_target_properties (Bullet3Common            PROPERTIES FOLDER Thirdparty)
set_target_properties (Bullet3Dynamics          PROPERTIES FOLDER Thirdparty)
set_target_properties (Bullet3Geometry          PROPERTIES FOLDER Thirdparty)
set_target_properties (Bullet3OpenCL_clew       PROPERTIES FOLDER Thirdparty)
set_target_properties (BulletInverseDynamics    PROPERTIES FOLDER Thirdparty)
set_target_properties (BulletSoftBody           PROPERTIES FOLDER Thirdparty)
set_target_properties (OpenAL32                 PROPERTIES FOLDER Thirdparty)
set_target_properties (SDL2main                 PROPERTIES FOLDER Thirdparty)
endif()