# bgfx common extension
# this library contains bgfx common features found in the bgfx/examples/common/ directory:
#
# imgui : imgui.cpp and imgui.h are rename to bgfximgui.cpp and bgfximgui.h
# debugdraw
# font

cmake_minimum_required(VERSION 3.0)
project (bgfxcommon)

# Third party libs
include( ../cmake/3rdparty/dear-imgui.cmake )

# Grab the bgfxcommon source files
file( GLOB BGFXCOMMON_SOURCES 
	debugdraw/*.cpp
	font/*.cpp
	imgui/*.cpp
	*.h
	*.cpp
)

add_library( bgfxcommon STATIC ${BGFXCOMMON_SOURCES} )

target_include_directories( bgfxcommon PUBLIC ${BX_DIR}/include )
target_include_directories( bgfxcommon PUBLIC ${BIMG_DIR}/include )
target_include_directories( bgfxcommon PUBLIC ${BGFX_DIR}/include )
target_include_directories( bgfxcommon PUBLIC ${BGFX_DIR}/3rdparty )

target_link_libraries( bgfxcommon PUBLIC dear-imgui )


