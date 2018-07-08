# detect os
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
endif()

# helper to copy include files to output directory
FUNCTION(COPY_HEADERS src dst)
    file(COPY ${src}
        DESTINATION ${dst} FILES_MATCHING REGEX "^.*.(h|hpp|inl)$"
        PATTERN ".svn" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE)
ENDFUNCTION(COPY_HEADERS)

# emscripten settings
if("${CMAKE_C_COMPILER}" MATCHES "emcc")
    set(EMSCRIPTEN ON)
    set(MULTI_THREAD OFF)
    set(TCMALLOC OFF)
    #set(CFLAGS_EMSCRIPTEN "-Oz -O3")
endif()

# flags for windows
if(WINDOWS)
    # msvc compiler
    if(MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
        add_definitions( /MP /wd"4305" )
    endif()
endif()
