cmake_minimum_required(VERSION 2.8)
project(efsw)

set(EFSW_DIR ${PROJECT_SOURCE_DIR}/.)


include_directories(${EFSW_DIR}/src)
include_directories(${EFSW_DIR}/include)

IF(LINUX)
SET (PLATFORM_SRC_FILES
    src/efsw/platform/posix/FileSystemImpl.cpp
    src/efsw/platform/posix/FileSystemImpl.hpp
    src/efsw/platform/posix/MutexImpl.cpp
    src/efsw/platform/posix/MutexImpl.hpp
    src/efsw/platform/posix/SystemImpl.cpp
    src/efsw/platform/posix/SystemImpl.hpp
    src/efsw/platform/posix/ThreadImpl.cpp
    src/efsw/platform/posix/ThreadImpl.hpp
    )
ENDIF(LINUX)

IF(WIN32)
SET (PLATFORM_SRC_FILES
    src/efsw/platform/win/FileSystemImpl.cpp
    src/efsw/platform/win/FileSystemImpl.hpp
    src/efsw/platform/win/MutexImpl.cpp
    src/efsw/platform/win/MutexImpl.hpp
    src/efsw/platform/win/SystemImpl.cpp
    src/efsw/platform/win/SystemImpl.hpp
    src/efsw/platform/win/ThreadImpl.cpp
    src/efsw/platform/win/ThreadImpl.hpp
    src/efsw/WatcherWin32.cpp
    src/efsw/WatcherWin32.hpp
    )
ENDIF(WIN32)

SET (SRC_FILES
    src/efsw/base.hpp
    src/efsw/Debug.cpp
    src/efsw/Debug.hpp
    src/efsw/DirectorySnapshot.cpp
    src/efsw/DirectorySnapshotDiff.cpp
    src/efsw/DirectorySnapshotDiff.hpp
    src/efsw/DirectorySnapshot.hpp
    src/efsw/DirWatcherGeneric.cpp
    src/efsw/DirWatcherGeneric.hpp
    src/efsw/FileInfo.cpp
    src/efsw/FileInfo.hpp
    src/efsw/FileSystem.cpp
    src/efsw/FileSystem.hpp
    src/efsw/FileWatcher.cpp
    src/efsw/FileWatcherFSEvents.cpp
    src/efsw/FileWatcherFSEvents.hpp
    src/efsw/FileWatcherGeneric.cpp
    src/efsw/FileWatcherGeneric.hpp
    src/efsw/FileWatcherImpl.cpp
    src/efsw/FileWatcherImpl.hpp
    src/efsw/FileWatcherInotify.cpp
    src/efsw/FileWatcherInotify.hpp
    src/efsw/FileWatcherKqueue.cpp
    src/efsw/FileWatcherKqueue.hpp
    src/efsw/FileWatcherWin32.cpp
    src/efsw/FileWatcherWin32.hpp
    src/efsw/Log.cpp
    src/efsw/Mutex.cpp
    src/efsw/Mutex.hpp
    src/efsw/sophist.h
    src/efsw/String.cpp
    src/efsw/String.hpp
    src/efsw/System.cpp
    src/efsw/System.hpp
    src/efsw/Thread.cpp
    src/efsw/Thread.hpp
    src/efsw/Utf.hpp
    src/efsw/Utf.inl
    src/efsw/Watcher.cpp
    src/efsw/WatcherFSEvents.cpp
    src/efsw/WatcherFSEvents.hpp
    src/efsw/WatcherGeneric.cpp
    src/efsw/WatcherGeneric.hpp
    src/efsw/Watcher.hpp
    src/efsw/WatcherInotify.cpp
    src/efsw/WatcherInotify.hpp
    src/efsw/WatcherKqueue.cpp
    src/efsw/WatcherKqueue.hpp
    #src/efsw/WatcherWin32.cpp
    #src/efsw/WatcherWin32.hpp
)

add_library(efsw STATIC ${PLATFORM_SRC_FILES} ${SRC_FILES})

#SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
install( DIRECTORY ${EFSW_DIR}/include/ DESTINATION include )
