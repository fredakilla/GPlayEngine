cmake_minimum_required(VERSION 3.1)
project(GPlayEngine)

include(global.cmake)

# set global settings
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_DEBUG_POSTFIX "")

# set output directories
set(OUT_DIR_BIN ${CMAKE_SOURCE_DIR}/output/bin)
set(OUT_DIR_LIB ${CMAKE_SOURCE_DIR}/output/lib)
set(OUT_DIR_INCLUDE ${CMAKE_SOURCE_DIR}/output/include)

# set cmake output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUT_DIR_LIB})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR_BIN})
if(WINDOWS)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()


add_subdirectory(src)
add_subdirectory(samples)



## copy engine headers to output directory
#COPY_HEADERS(src/ ${OUT_DIR_INCLUDE}/gplayengine/)
#
## write setup.pri file for Qt
#file(WRITE setup.pri "OUTPUT_DIR=${CMAKE_SOURCE_DIR}/output")
#
## copy res directory to output bin directory (or create symbolic link on supported systems)
#file(MAKE_DIRECTORY ${OUT_DIR_BIN})
#if(WINDOWS)
#    # add copy command
#elseif(LINUX)
#    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/res ${OUT_DIR_BIN}/res)
#endif()

