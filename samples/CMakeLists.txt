set(CMAKE_CXX_STANDARD 11)

include_directories(
        ${OUT_DIR_INCLUDE}/gplayengine
        ${EXTERNAL_DEPS_INCLUDE}/gplayengine/thirdparty
)

link_directories(${OUT_DIR_LIB} ${EXTERNAL_DEPS_LIB})

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
endif()

add_subdirectory(minimal)
add_subdirectory(browser)
add_subdirectory(spaceship)
add_subdirectory(racer)
add_subdirectory(character)
add_subdirectory(graphics)
