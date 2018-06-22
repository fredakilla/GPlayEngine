#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "../core/Base.h"
#include "../graphics/Image.h"
#include "../graphics/Texture.h"

namespace gplay {

class FrameBuffer : public Ref
{
public:

    /**
     * Destructor.
     */
     ~FrameBuffer();

    /**
     * Create frame buffer (simple).
     *
     * @param id The id of the frame buffer.
     * @param width Texture width.
     * @param height Texture height.
     * @param format Texture format.
     * @return new created frame buffer.
     */
    static FrameBuffer* create(const char* id, uint16_t width, uint16_t height, Texture::Format format);

    /**
     * Create MRT frame buffer from textures vector.
     *
     * @param id The id of the frame buffer.
     * @param textures Textures attachments.
     * @return new created frame buffer.
     */
    static FrameBuffer* create(const char *id, std::vector<Texture*> textures);

    /**
     * Bind frame buffer.
     * Use this before rendering scene to render into the frame buffer.
     */
    void bind();

    /**
     * Get a render target attached to this frame buffer.
     *
     * @param id Index of the render target.
     * @return Sampler of the render target.
     */
    Texture::Sampler* getRenderTarget(uint16_t id);

    /**
     * Get a render target attached to this frame buffer.
     *
     * @param id Name of the render target.
     * @return Sampler of the render target.
     */
    Texture::Sampler* getRenderTarget(std::string id);

    /**
     * Get a named FrameBuffer from its ID.
     *
     * @param id The ID of the FrameBuffer to search for.
     *
     * @return The FrameBuffer with the specified ID, or NULL if one was not found.
     */
    static FrameBuffer* getFrameBuffer(const char* id);

    /**
     * Get the ID of this FrameBuffer.
     *
     * @return The ID of this FrameBuffer.
     */
    const char* getId() const;

private:
    FrameBuffer(const char* id);

    bgfx::FrameBufferHandle _frameBufferHandle;
    std::vector<Texture::Sampler*> _samplers;
    std::string _id;
};

}

#endif
