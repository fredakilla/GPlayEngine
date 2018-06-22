#ifndef GP_NO_PLATFORM
#ifdef _WIN32

#include "gplay-engine.h"

using namespace gplay;

#ifndef _WINDOWS_
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

extern int __app_argc;
extern char** __app_argv;

#include <SDL2/SDL.h>
#undef main
#define main SDL_main

/**
 * Main entry point.
 */
//extern "C" int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
int main(int argc, char *argv[])
{
    __app_argc = argc;
    __app_argv = argv;

    Game* game = Game::getInstance();

    Platform* platform = Platform::create(game);
    GP_ASSERT(platform);

    platform->start();
    while(platform->processEvents())
    {
        platform->frame();
    }
    platform->stop();

    delete platform;
    return 0;
}

#endif
#endif
