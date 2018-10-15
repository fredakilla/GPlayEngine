############################################################################
#                                                                          #
#  file: CMakeLists.txt                                                    #
#  Copyright (C) 2016                                                      #
#      Enrico Bertolazzi                                                   #
#      Dipartimento di Ingegneria Industriale                              #
#      Universita` degli Studi di Trento                                   #
#      email: enrico.bertolazzi@ing.unitn.it                               #
#                                                                          #
############################################################################

cmake_minimum_required(VERSION 3.5)

SET( TARGET Splines )

PROJECT( ${TARGET} CXX C )

SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
SET( CMAKE_VERBOSE_MAKEFILE            TRUE )
SET( CMAKE_BUILD_TYPE                  Release )

IF( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC )
  SET( CMAKE_CXX_FLAGS "-Wall -Wcast-align" )
  IF ( APPLE )
    SET( CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -funroll-loops " )
    SET( CMAKE_CXX_FLAGS_DEBUG   "-O1 -g -DDEBUG " )
  ELSE()
    SET( CMAKE_CXX_FLAGS_RELEASE "-fPIC -fno-common -O3 -g0 -funroll-loops " )
    SET( CMAKE_CXX_FLAGS_DEBUG   "-fPIC -fno-common -O1 -g -DDEBUG " )
  ENDIF()
ELSEIF( MSVC )
  SET( VSFLAGS_COMMON "/nologo /GS /W3 /WX- /Gm- /Gd /fp:precise /EHsc /FS /D_WINDOWS /D_CRT_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS /DHAVE_STRING_H /DNO_GETTIMEOFDAY" )
  SET( CMAKE_CXX_FLAGS_RELEASE "/O2 /MD ${VSFLAGS_COMMON}" )
  SET( CMAKE_CXX_FLAGS_DEBUG   "/Od /Ob0 /MDd /Zi /RTC1 /D_DEBUG ${VSFLAGS_COMMON}" )
ENDIF()

SET( CMAKE_C_FLAGS         ${CMAKE_CXX_FLAGS} )
SET( CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE} )
SET( CMAKE_C_FLAGS_DEBUG   ${CMAKE_CXX_FLAGS_DEBUG} )

SET( SOURCES )
FILE( GLOB S ./src/*.cc )
FOREACH (F ${S})
  FILE( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  LIST( APPEND SOURCES ${RF} )
ENDFOREACH (F ${S})
SET( HEADERS )
FILE( GLOB S ./src/*.hh )
FOREACH (F ${S})
  FILE( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  LIST( APPEND HEADERS ${RF} )
ENDFOREACH (F ${S})

INCLUDE_DIRECTORIES( src GC/lib/include )

ADD_LIBRARY( ${TARGET} STATIC ${SOURCES} ${HEADERS} )

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH    ${CMAKE_CURRENT_SOURCE_DIR}/lib)
file(MAKE_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/bin )
file(MAKE_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/lib )

INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/include )

IF( BUILD_EXECUTABLE )
  ADD_EXECUTABLE( test1 tests/test1.cc ${HEADERS} )
  TARGET_LINK_LIBRARIES( test1 ${TARGET} )
  
  ADD_EXECUTABLE( test2 tests/test2.cc ${HEADERS} )
  TARGET_LINK_LIBRARIES( test1 ${TARGET} )
  
  ADD_EXECUTABLE( test3 tests/test3.cc ${HEADERS} )
  TARGET_LINK_LIBRARIES( test3 ${TARGET} )
  
  ADD_EXECUTABLE( test4 tests/test4.cc ${HEADERS} )
  TARGET_LINK_LIBRARIES( test4 ${TARGET} )
  
  ADD_EXECUTABLE( test5 tests/test5.cc ${HEADERS} )
  TARGET_LINK_LIBRARIES( test5 ${TARGET} )
  
  ADD_EXECUTABLE( test6 tests/test6.cc ${HEADERS} )
  TARGET_LINK_LIBRARIES( test6 ${TARGET} )
  
  ADD_EXECUTABLE( test7 tests/test7.cc ${HEADERS} )
  TARGET_LINK_LIBRARIES( test7 ${TARGET} )
ENDIF()

MESSAGE( STATUS "Using ${SSE_FLAGS} extensions")
MESSAGE( STATUS "C compiler                  = ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER}" )
MESSAGE( STATUS "C++ compiler                = ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_RELEASE     = ${CMAKE_CXX_FLAGS_RELEASE}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS             = ${CMAKE_CXX_FLAGS}" )
MESSAGE( STATUS "CMAKE_C_FLAGS_RELEASE       = ${CMAKE_C_FLAGS_RELEASE}" )
MESSAGE( STATUS "CMAKE_C_FLAGS               = ${CMAKE_C_FLAGS}" )
MESSAGE( STATUS "CMAKE_SYSTEM_NAME           = ${CMAKE_SYSTEM_NAME}" )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR      = ${CMAKE_SYSTEM_PROCESSOR}" )
IF(APPLE)
  MESSAGE( STATUS "CMAKE_OSX_SYSROOT           = ${CMAKE_OSX_SYSROOT}" )
  MESSAGE( STATUS "CMAKE_OSX_ARCHITECTURES     = ${CMAKE_OSX_ARCHITECTURES}" )
  MESSAGE( STATUS "CMAKE_OSX_DEPLOYMENT_TARGET = ${CMAKE_OSX_DEPLOYMENT_TARGET}" )
ENDIF()
