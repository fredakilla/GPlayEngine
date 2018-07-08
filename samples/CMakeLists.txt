set(CMAKE_CXX_STANDARD 11)

include_directories(
        ${OUT_DIR_INCLUDE}/gplayengine
        ${OUT_DIR_INCLUDE}/gplayengine/thirdparty
)

link_directories(${OUT_DIR_LIB})

if(LINUX)

    add_definitions(-D__linux__)

    set(GPLAY_LIBRARIES 
        gplay
        gplay-deps
        m
        GL
        rt
        dl
        X11
        pthread
        sndio)

elseif(WINDOWS)

    include_directories(${OUT_DIR_INCLUDE}/gplayengine/thirdparty/compat/msvc)

    set(GPLAY_LIBRARIES
            gplay
            gplay-deps            
            opengl32
            kernel32
            user32
            winmm
            gdi32
            winspool
            comdlg32
            advapi32
            shell32
            imm32
            ole32
            oleaut32
            uuid
            version
            odbc32
            odbccp32)

elseif(EMSCRIPTEN)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2  -s USE_FREETYPE=1 -s TOTAL_MEMORY=268435456")
        #-s DISABLE_EXCEPTION_CATCHING=0
        #-s DEMANGLE_SUPPORT=1
        #-s SAFE_HEAP=1 --bind
        #-s WASM=1
        #-s TOTAL_MEMORY=268435456
        #-s ALLOW_MEMORY_GROWTH=1
        #-s FULL_ES2=1


    set(GPLAY_LIBRARIES
        gplay
        gplay-deps
    )

endif()

add_subdirectory(minimal)
add_subdirectory(browser)
add_subdirectory(spaceship)
add_subdirectory(racer)
add_subdirectory(character)
add_subdirectory(graphics)
