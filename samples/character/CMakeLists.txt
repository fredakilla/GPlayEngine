set(GAME_NAME sample-character)

set(GAME_SRC
    src/CharacterGame.cpp
    src/CharacterGame.h
)

add_executable(${GAME_NAME} ${GAME_SRC})

target_link_libraries(${GAME_NAME} ${GPLAY3D_LIBRARIES})

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/game.dds.config ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GAME_NAME}.config COPYONLY )

if(MSVC)
set_target_properties(${GAME_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()
