cmake_minimum_required(VERSION 2.8)

add_subdirectory(_internal/Source)
add_subdirectory(_internal/Dependencies)

install(FILES libjson.h JSONOptions.h DESTINATION include/libjson)
