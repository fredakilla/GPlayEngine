cmake_minimum_required(VERSION 3.1)
project(GPlayEngine)

# detect os
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
endif()

# set global settings
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_DEBUG_POSTFIX "")

# set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libtmp)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libtmp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WINDOWS)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

set(OUT_DIR_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(OUT_DIR_LIB ${CMAKE_BINARY_DIR}/lib)
set(OUT_DIR_LIB_DEPS ${CMAKE_BINARY_DIR}/lib/thirdparty)
set(OUT_DIR_INCLUDE ${CMAKE_BINARY_DIR}/include)



# helper to copy include files to output directory
FUNCTION(COPY_HEADERS src dst)
    file(COPY ${src}
        DESTINATION ${dst} FILES_MATCHING REGEX "^.*.(h|hpp|inl)$"
        PATTERN ".svn" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE)
ENDFUNCTION(COPY_HEADERS)


add_subdirectory(thirdparty)
add_subdirectory(src)
add_subdirectory(samples)

# copy engine headers to output build directory
COPY_HEADERS(src/ ${OUT_DIR_INCLUDE}/gplayengine/)

# write setup.pri file for Qt
file(WRITE setup.pri "BUILD_DIR=${CMAKE_BINARY_DIR}")

# copy res directory to output bin directory (or create symbolic link on supported systems)
file(MAKE_DIRECTORY ${OUT_DIR_BIN})
if(WINDOWS)
    # add copy command
elseif(LINUX)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/res ${OUT_DIR_BIN}/res)
endif()

