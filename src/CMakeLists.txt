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
    ${PROJECT_SOURCE_DIR}/editor/*.cpp
)

add_library(GPLAY STATIC
    ${GPLAY_SRC}
)

set_target_properties(GPLAY PROPERTIES
    OUTPUT_NAME gplay
)
