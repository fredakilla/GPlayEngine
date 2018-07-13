#ifndef GP_NO_PLATFORM
#if defined(EMSCRIPTEN)

#include "gplay-engine.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#endif

using namespace gplay;

extern int __app_argc;
extern char** __app_argv;


void emloop(void* arg_)
{
    Platform* plateform = (Platform*)arg_;
    plateform->processEvents();
    plateform->frame();
}


/**
 * Main entry point.
 */
int main(int argc, char** argv)
{
    __app_argc = argc;
    __app_argv = argv;

    Game* game = Game::getInstance();

    Platform* platform = Platform::create(game);
    GP_ASSERT(platform);

    platform->start();

    emscripten_set_main_loop_arg(emloop, (void*)platform , 60, 1);

    platform->stop();

    delete platform;
    return 0;
}

#endif
#endif
