cmake_minimum_required(VERSION 2.8)

set(LIBJSON_DEPS_HDR libbase64++/libbase64++.h mempool++/mempool.h)

install(FILES ${LIBJSON_DEPS_HDR} DESTINATION include/libjson/_internal/Source)
