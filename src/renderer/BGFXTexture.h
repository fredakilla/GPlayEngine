#ifndef BGFXTEXTUREHANDLE_H
#define BGFXTEXTUREHANDLE_H

#include "../core/Base.h"
#include "../graphics/Texture.h"

namespace gplay {

class BGFXTexture
{
    friend class FrameBuffer;

public:
    ~BGFXTexture();

    static Texture* createFromFile(const char* path);
    static Texture* createFromData(Texture::TextureInfo info, const unsigned char* data = nullptr, uint32_t flags = BGFX_TEXTURE_NONE);

    void bind(Uniform* uniform, Texture* texture, uint32_t customFlags = BGFX_TEXTURE_NONE);
    const bgfx::TextureHandle getHandle() const { return _handle; }
    static bgfx::TextureFormat::Enum toBgfxFormat(Texture::Format gp3dFormat);
    static Texture::Format toGp3dFormat(bgfx::TextureFormat::Enum bimgTextureFormat);

private:

    BGFXTexture();

    bgfx::TextureHandle _handle;
};

} // end namespace gplay

#endif // BGFXTEXTUREHANDLE_H
