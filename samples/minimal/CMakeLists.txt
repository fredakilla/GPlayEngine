cmake_minimum_required(VERSION 3.1)
project(sample-minimal)

set(GAME_NAME ${PROJECT_NAME})
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

set(GAME_SRC
    src/main.h
    src/main.cpp
)

add_executable(${GAME_NAME} ${GAME_SRC})
target_link_libraries(${GAME_NAME} ${GPLAY_LIBRARIES})
add_dependencies(${GAME_NAME} GPLAY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/game.config ${OUT_DIR_BIN}/${GAME_NAME}.config COPYONLY)

if(EMSCRIPTEN)
    SET_TARGET_PROPERTIES(${GAME_NAME} PROPERTIES LINK_FLAGS "--preload-file ${OUT_DIR_BIN}/emdata@/ --bind")
endif()

if(MSVC)
    set_target_properties(${GAME_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${OUT_DIR_BIN}")
endif()

