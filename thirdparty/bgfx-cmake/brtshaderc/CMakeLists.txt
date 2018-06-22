cmake_minimum_required(VERSION 3.0)
project (brtshaderc)

include( ../cmake/3rdparty/fcpp.cmake )
include( ../cmake/3rdparty/glsl-optimizer.cmake )
include( ../cmake/3rdparty/glslang.cmake )

add_library( brtshaderc
    brtshaderc.cpp
)

target_include_directories( brtshaderc PUBLIC ${CMAKE_CURRENT_LIST_DIR} )
target_compile_definitions( brtshaderc PRIVATE "-D_CRT_SECURE_NO_WARNINGS" )

target_link_libraries( brtshaderc
    bx
    bimg
    bgfx-vertexdecl
    bgfx-shader-spirv
    fcpp
    glsl-optimizer
    glslang
)

install(FILES brtshaderc.h DESTINATION include/brtshaderc )