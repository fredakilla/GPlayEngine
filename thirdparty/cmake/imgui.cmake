add_library(
	imgui
	STATIC
	${PROJECT_SOURCE_DIR}/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/imgui/imgui_widgets.cpp
)
target_include_directories( imgui PUBLIC ${PROJECT_SOURCE_DIR}/imgui )
set_target_properties( imgui PROPERTIES FOLDER "imgui" )

set(IMGUI_PUBLIC_HEADERS 
	${PROJECT_SOURCE_DIR}/imgui/imconfig.h
	${PROJECT_SOURCE_DIR}/imgui/imgui.h
	${PROJECT_SOURCE_DIR}/imgui/imgui_internal.h
)

install(FILES ${IMGUI_PUBLIC_HEADERS} DESTINATION include/imgui)
