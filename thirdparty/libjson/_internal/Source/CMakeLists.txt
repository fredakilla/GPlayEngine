cmake_minimum_required(VERSION 2.8)

set(LIBJSON_SRC internalJSONNode.cpp JSONAllocator.cpp JSONChildren.cpp
    JSONDebug.cpp JSONIterators.cpp JSONMemory.cpp JSONNode.cpp
    JSONNode_Mutex.cpp JSONPreparse.cpp JSONStream.cpp JSONValidator.cpp
    JSONWorker.cpp JSONWriter.cpp libjson.cpp)

set(LIBJSON_HDR JSONAllocator.h  JSONMemoryPool.h JSONStream.h
        JSONChildren.h  JSONNode.h  JSONValidator.h
        JSONDebug.h  JSONPreparse.h  JSONWorker.h
        JSONDefs.h  JSONSharedString.h JSON_Base64.h
        JSONGlobals.h  JSONSingleton.h  NumberToString.h
        JSONMemory.h  JSONStats.h  internalJSONNode.h
        JSONDefs/GNU_C.h JSONDefs/Strings_Defs.h JSONDefs/Unknown_C.h
        JSONDefs/Visual_C.h)

if(APPLE)
    add_definitions(-fast -ffast-math -fexpensive-optimizations -DNDEBUG)
else()
    add_definitions(-O3 -ffast-math -fexpensive-optimizations -DNDEBUG)
endif(APPLE)

add_library(json ${LIBJSON_SRC})

install(FILES ${LIBJSON_HDR} DESTINATION include/libjson/_internal/Source)
