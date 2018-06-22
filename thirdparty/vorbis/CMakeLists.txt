cmake_minimum_required(VERSION 2.8)

set(VORBIS_PUBLIC_HEADERS include/vorbis/codec.h include/vorbis/vorbisenc.h include/vorbis/vorbisfile.h)

if(WIN32)
	add_definitions(-DUSE_MEMORY_H)
else(WIN32)
	add_definitions(-DHAVE_ALLOCA_H -DUSE_MEMORY_H)
endif(WIN32)
add_definitions(-w)

set(VORBIS_SRC 
	./lib/synthesis.c
	./lib/block.c
	./lib/floor0.c
	./lib/lookup.c
	./lib/analysis.c
	./lib/sharedbook.c
	./lib/window.c
	./lib/info.c
	./lib/mdct.c
	./lib/smallft.c
	./lib/codebook.c
	./lib/lpc.c
	./lib/mapping0.c
	./lib/floor1.c
	./lib/bitrate.c
	./lib/res0.c
	./lib/psy.c
	./lib/envelope.c
	./lib/lsp.c
	./lib/registry.c)

add_library(vorbis ${VORBIS_SRC})
add_library(vorbisfile ./lib/vorbisfile.c)
add_library(vorbisenc ./lib/vorbisenc.c)

include_directories(include)
include_directories(lib)

# For ogg headers
include_directories("${PROJECT_SOURCE_DIR}/ogg/include")

# For ogg configured headers
include_directories("${PROJECT_BINARY_DIR}/ogg/include")

install(FILES ${VORBIS_PUBLIC_HEADERS} DESTINATION include/vorbis)
