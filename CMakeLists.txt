cmake_minimum_required(VERSION 3.1)
project(gplay)

# detect os
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
endif()

# set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(OUT_DIR_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(OUT_DIR_LIB ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(OUT_DIR_INCLUDE ${CMAKE_BINARY_DIR}/include)



# helper to copy include files to output directory
FUNCTION(COPY_HEADERS src dst)
    file(COPY ${src}
        DESTINATION ${dst} FILES_MATCHING REGEX "^.*.(h|hpp|inl)$"
        PATTERN ".svn" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE)
ENDFUNCTION(COPY_HEADERS)

# build thirdparty
add_subdirectory(thirdparty)

# build gplayengine
add_subdirectory(src)

add_subdirectory(samples)

# copy headers to build
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

