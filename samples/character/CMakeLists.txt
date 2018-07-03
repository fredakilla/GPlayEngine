cmake_minimum_required(VERSION 3.1)
project(sample-character)

set(GAME_NAME ${PROJECT_NAME})

set(GAME_SRC
    src/CharacterGame.cpp
    src/CharacterGame.h
)

add_executable(${GAME_NAME} ${GAME_SRC})
target_link_libraries(${GAME_NAME} ${GPLAY_LIBRARIES})
add_dependencies(${GAME_NAME} GPLAY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/game.dds.config ${OUT_DIR_BIN}/${GAME_NAME}.config COPYONLY )

if(MSVC)
	set_target_properties(${GAME_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${OUT_DIR_BIN}")
endif()
