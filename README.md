# GPlayEngine
GPlayEngine is a C++ cross-platform game engine for creating 2D/3D games.

GPlayEngine is based on the [GamePlay 3D engine](http://www.gameplay3d.io/) v3.0 and principally replace the OpenGL 2.x backend to use the [BGFX](https://github.com/bkaradzic/bgfx) graphic framework.


## Screenshots
<img src="https://i.imgur.com/u3arwg3.png" width="50%" height="%"><img src="https://i.imgur.com/IZKGhDb.jpg" width="50%" height="%">
<img src="https://i.imgur.com/0ei9Y28.png" width="50%" height="%"><img src="https://i.imgur.com/mXvz27x.jpg" width="50%" height="%">
<img src="https://i.imgur.com/nRpTNIm.jpg" width="50%" height="%"><img src="https://i.imgur.com/SDIgTkt.png" width="50%" height="%">
<img src="https://i.imgur.com/KyRqyIP.png" width="25%" height="%">

## Current status
- Cmake is used as main build system (qtcreator is also up-to-date)
- bgfx integration is completed and replace opengl.
- bgfx is currently forced to use the opengl driver because engine still use some old glsl shaders. A pass on shaders is planned to use the bgfx shader syntax to be fully compatible with others bgfx backends.
- Engine now use SDL2 by default to manage windows and inputs. Gamepad are not yet implanted.
- Lua is temporarily disabled to speed up the compilation during core dev phase.
- All previous samples and demo are now working with the new renderer.
- A new sample "sample-graphics" is used to test bgfx conversion and new features.
- Only tested on Linux, Windows and Emscripten for now.


## Install submodules dependencies

Some thirdparty libraries are registered as submodules, you need to install them before building engine.
```
git submodule init
git submodule update
```

## Building
See [wiki - compiling framework](https://github.com/fredakilla/GPlayEngine/wiki/Compiling-framework) for build instructions.


## Features
- BGFX based rendering system.
- Scene graph system with support for lights, cameras, audio, physics, and drawables.
- Declarative scene, animation, particles and material bindings.
- Material system with built-in shader library (forward rendering).
- Post-processing.
- Physics using Bullet.
- Particle effects with built-in particle system or SPARK engine system.
- Height map based terrains with multiple surface layers and LOD.
- Easy-to-use sprite, tileset and text rendering.
- Declarative UI system supporing 2D/3D theme-able user interfaces.
- Nice, customizable and complete built-in 2D GUI, with buttons, lists, edit boxes, layout...
- ImGui GUI support.
- File watcher system for hot reloading (shaders, scripts...)
- Fully extensible animation system with skeletal character animation.
- Complete 3D audio system with WAV and OGG support.
- AI agent, state machine and messaging.
- Event messaging system.
- Full vector math library with 2D/3D math and visibility culling.
- Mouse, keyboard, touch, gestures and gamepad support.
- Lua script bindings and user binding generator tool.
- Binary encoding tool for creating optimized asset bundles for TTF fonts and 3D FBX assets.
- Documented using doxygen.
- Image supported format (DDS, KTX, PVR, JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC)
- 3D model supported format (FBX)

## License

Open-source under Apache 2.0 license.
