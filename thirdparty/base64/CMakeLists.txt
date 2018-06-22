cmake_minimum_required(VERSION 2.8)

set(BASE64_PUBLIC_HEADERS base64.h)

set(BASE64_SRC base64.cpp)

add_library(base64 ${BASE64_SRC})

install(FILES ${BASE64_PUBLIC_HEADERS} DESTINATION include/base64)
