cmake_minimum_required(VERSION 2.8)

set(OGG_SRC src/framing.c src/bitwise.c)
set(OGG_PUBLIC_HEADERS include/ogg/ogg.h include/ogg/os_types.h)

add_definitions(-w)

# Normally, these are set via ./configure
set(INCLUDE_INTTYPES_H 1)
set(INCLUDE_STDINT_H 1)
set(INCLUDE_SYS_TYPES_H 1)
set(SIZE16 int16_t)
set(USIZE16 uint16_t)
set(SIZE32 int32_t)
set(USIZE32 uint32_t)
set(SIZE64 int64_t)
configure_file(include/ogg/config_types.h.in include/ogg/config_types.h)
include_directories("${PROJECT_BINARY_DIR}/ogg/include")

include_directories(include)
add_library(ogg ${OGG_SRC})

install(FILES ${OGG_PUBLIC_HEADERS} DESTINATION include/ogg)
install(FILES "${PROJECT_BINARY_DIR}/ogg/include/ogg/config_types.h" DESTINATION include/ogg)
