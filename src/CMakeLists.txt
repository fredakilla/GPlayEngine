cmake_minimum_required(VERSION 3.1)
project(gplay)

set(CMAKE_CXX_STANDARD 11)

# disable lua binding for now
add_definitions(-DGP_NO_LUA_BINDINGS)

if(WINDOWS)
    include_directories(${OUT_DIR_INCLUDE}/gplayengine/thirdparty/bx/compat/msvc)
endif()

include_directories(
    ${OUT_DIR_INCLUDE}/gplayengine/
    ${OUT_DIR_INCLUDE}/gplayengine/thirdparty
)

file(GLOB GPLAY_SRC
    ${PROJECT_SOURCE_DIR}/ai/*.cpp
    ${PROJECT_SOURCE_DIR}/animation/*.cpp
    ${PROJECT_SOURCE_DIR}/audio/*.cpp
    ${PROJECT_SOURCE_DIR}/core/*.cpp
    ${PROJECT_SOURCE_DIR}/events/*.cpp
    ${PROJECT_SOURCE_DIR}/graphics/*.cpp
    ${PROJECT_SOURCE_DIR}/input/*.cpp
    ${PROJECT_SOURCE_DIR}/math/*.cpp
    ${PROJECT_SOURCE_DIR}/physics/*.cpp
    ${PROJECT_SOURCE_DIR}/renderer/*.cpp
    ${PROJECT_SOURCE_DIR}/script/*.cpp
    ${PROJECT_SOURCE_DIR}/sparkparticles/*.cpp
    ${PROJECT_SOURCE_DIR}/ui/*.cpp
)

add_library(GPLAY STATIC
    ${GPLAY_SRC}
)




set(libdir "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
set(outdir "${OUT_DIR_LIB}")

if(WINDOWS)
    string(REPLACE "/" "\\" libdir "${libdir}")
    string(REPLACE "/" "\\" outdir "${outdir}")
endif()

set_target_properties(GPLAY PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${outdir}"
    ARCHIVE_OUTPUT_DIRECTORY "${outdir}"
    OUTPUT_NAME gplay
)

# On Windows, move gplay.lib to output lib directory using custom command
# set target properties do not work on my Windows...
if(WINDOWS)
    ADD_CUSTOM_COMMAND(
        TARGET GPLAY
        POST_BUILD
        COMMAND move ARGS "${srcdir}\\gplay.lib" "${outdir}\\gplay.lib"
    )
endif()

add_dependencies(GPLAY GPLAY_DEPS)

