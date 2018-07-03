cmake_minimum_required(VERSION 3.1)
project(sample-browser)

set(GAME_NAME ${PROJECT_NAME})

set(GAME_SRC
    src/Audio3DSample.cpp
    src/Audio3DSample.h
    src/AudioSample.cpp
    src/AudioSample.h
    src/BillboardSample.cpp
    src/BillboardSample.h
    src/FirstPersonCamera.cpp
    src/FirstPersonCamera.h
    src/FontSample.cpp
    src/FontSample.h
    src/FormsSample.cpp
    src/FormsSample.h
    src/GamepadSample.cpp
    src/GamepadSample.h
    src/GestureSample.cpp
    src/GestureSample.h
    src/Grid.cpp
    src/Grid.h
    src/InputSample.cpp
    src/InputSample.h
    src/LightSample.cpp
    src/LightSample.h
    src/MeshBatchSample.cpp
    src/MeshBatchSample.h
    src/MeshPrimitiveSample.cpp
    src/MeshPrimitiveSample.h
    src/ParticlesSample.cpp
    src/ParticlesSample.h
    src/PhysicsCollisionObjectSample.cpp
    src/PhysicsCollisionObjectSample.h
    src/PostProcessSample.cpp
    src/PostProcessSample.h
    src/Sample.cpp
    src/Sample.h
    src/SamplesGame.cpp
    src/SamplesGame.h
    src/SceneCreateSample.cpp
    src/SceneCreateSample.h
    src/SceneLoadSample.cpp
    src/SceneLoadSample.h
    src/SpriteBatchSample.cpp
    src/SpriteBatchSample.h
    src/SpriteSample.cpp
    src/SpriteSample.h
    src/TerrainSample.cpp
    src/TerrainSample.h
    src/TextureSample.cpp
    src/TextureSample.h
    src/TriangleSample.cpp
    src/TriangleSample.h
    src/WaterSample.cpp
    src/WaterSample.h
)

add_executable(${GAME_NAME} ${GAME_SRC})
target_link_libraries(${GAME_NAME} ${GPLAY_LIBRARIES})
add_dependencies(${GAME_NAME} GPLAY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/game.config ${OUT_DIR_BIN}/${GAME_NAME}.config COPYONLY)

if(MSVC)
    set_target_properties(${GAME_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${OUT_DIR_BIN}")
endif()
