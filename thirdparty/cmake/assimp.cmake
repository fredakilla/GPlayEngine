# assimp options
set( BUILD_SHARED_LIBS OFF CACHE BOOL "Build package with shared libraries." FORCE )
set( ASSIMP_NO_EXPORT ON CACHE BOOL "Disable Assimp's export functionality." FORCE )
set( ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "If the supplementary tools for Assimp are built in addition to the library." FORCE )
set( ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "If the official samples are built as well (needs Glut)." FORCE )
set( ASSIMP_BUILD_TESTS OFF CACHE BOOL "If the test suite for Assimp is built in addition to the library." FORCE )
set( ASSIMP_INSTALL_PDB OFF CACHE BOOL "Install MSVC debug files." FORCE )
set( ASSIMP_INSTALL_PDB OFF CACHE BOOL "Install MSVC debug files." FORCE )
set( ASSIMP_BUILD_ASSIMP_TOOLS OFF )

add_subdirectory(assimp)
