#include "../renderer/BGFXRenderer.h"
#include "../math/Transform.h"
#include "../core/Game.h"
#include "../renderer/BGFXGpuProgram.h"
#include "../graphics/View.h"

#include <bgfx/bgfx.h>

#define GP_DRAW_DEBUG   // allow debug text

namespace gplay {

BGFXRenderer *BGFXRenderer::_instance = nullptr;

BGFXRenderer::BGFXRenderer()
{
    printf("BGFXRenderer Created\n");

    _debug_flags = BGFX_DEBUG_TEXT;
    _reset_flags = BGFX_RESET_NONE;

    GP_ASSERT(!_instance); // Instance already exists
    _instance = this;
}

BGFXRenderer::~BGFXRenderer()
{
    print("BGFXRenderer Deleted\n");
}

void BGFXRenderer::initInstance()
{
    new BGFXRenderer;
}

void BGFXRenderer::resize(unsigned int width, unsigned height)
{
    _width = width;
    _height = height;

    bgfx::reset(_width, _height, _reset_flags);
}

void BGFXRenderer::toggleDebugStats()
{
    if ((_debug_flags & BGFX_DEBUG_STATS) == 0)
        _debug_flags |= BGFX_DEBUG_STATS;
    else
        _debug_flags &= ~BGFX_DEBUG_STATS;

    bgfx::setDebug(_debug_flags);
}

void BGFXRenderer::toggleWireFrame()
{
    if ((_debug_flags & BGFX_DEBUG_WIREFRAME) == 0)
        _debug_flags |= BGFX_DEBUG_WIREFRAME;
    else
        _debug_flags &= ~BGFX_DEBUG_WIREFRAME;

    bgfx::setDebug(_debug_flags);
}

void BGFXRenderer::toggleVSync()
{
    if (_isVsync)
        setVSync(false);
    else
        setVSync(true);
}

void BGFXRenderer::setVSync(bool enable)
{
    _isVsync = enable;

    if (enable)
        _reset_flags |= BGFX_RESET_VSYNC;
    else
        _reset_flags &= ~BGFX_RESET_VSYNC;

    bgfx::reset(_width, _height, _reset_flags);
}

void BGFXRenderer::setPaletteColor(unsigned char index, Vector4 color)
{
    bgfx::setPaletteColor(index, color.x, color.y, color.z, color.w);
}

void BGFXRenderer::queryCaps()
{
    // Query caps and limits.
    _caps._maxFrameBufferAttachments = bgfx::getCaps()->limits.maxFBAttachments;
}

void BGFXRenderer::beginFrame()
{
#ifdef GP_DRAW_DEBUG
    bgfx::dbgTextClear();
#endif
}

void BGFXRenderer::endFrame()
{
    bgfx::frame();
}

void BGFXRenderer::submit(const BGFXGpuProgram *gpuProgram)
{
    GP_ASSERT(gpuProgram && bgfx::isValid(gpuProgram->getProgram()));

    bgfx::submit(View::getCurrentViewId(), gpuProgram->getProgram());
}


} // end namespace gplay
