# bgfxEx.cmake - bgfx extensions building in cmake
# this library contains bgfx common features :
# dear-imgui
# debugdraw
# font


# Third party libs
include( cmake/3rdparty/dear-imgui.cmake )

# Grab the bimg source files
file( GLOB BGFXEX_SOURCES 
	${BGFX_DIR}/examples/common/debugdraw/*.cpp
	${BGFX_DIR}/examples/common/font/*.cpp
	${BGFX_DIR}/examples/common/imgui/*.cpp
	${BGFX_DIR}/examples/common/*.cpp
)


add_library( bgfxEx STATIC ${BGFXEX_SOURCES} )

target_include_directories( bgfxEx PUBLIC ${BX_DIR}/include )
target_include_directories( bgfxEx PUBLIC ${BIMG_DIR}/include )
target_include_directories( bgfxEx PUBLIC ${BGFX_DIR}/include )
#target_include_directories( bgfxEx PUBLIC ${BGFX_DIR}/3rdparty/dear-imgui )


target_link_libraries( bgfxEx PUBLIC dear-imgui )
