cmake_minimum_required(VERSION 3.1)
project(sample-graphics)

set(GAME_NAME ${PROJECT_NAME})


set(GAME_SRC
    src/SamplesGame.h
    src/DrawStress.h
    src/DynamicMesh.h
    src/Instancing.h
    src/Example.h
    src/Cube.h
    src/FirstPersonCamera.h
    src/RenderToTexture.h
    src/ImGuiTest.h
    src/SparkDemo.h
    src/SamplesGame.cpp
    src/DrawStress.cpp
    src/DynamicMesh.cpp
    src/Instancing.cpp
    src/Example.cpp
    src/Cube.cpp
    src/Sponza.cpp
    src/FirstPersonCamera.cpp
    src/RenderToTexture.cpp
    src/ImGuiTest.cpp
    src/SparkDemo.cpp
    src/Transparency.cpp
    src/ShaderReload.cpp
    src/EventSample.cpp
    src/LightsAndShadows.cpp
    src/DeferredTest.cpp
    src/DebugDraw.cpp
    src/EditorSample.cpp
)

add_executable(${GAME_NAME} ${GAME_SRC})
target_link_libraries(${GAME_NAME} ${GPLAY_LIBRARIES})
add_dependencies(${GAME_NAME} GPLAY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/game.config ${OUT_DIR_BIN}/${GAME_NAME}.config COPYONLY)

if(MSVC)
    set_target_properties(${GAME_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${OUT_DIR_BIN}")
endif()
