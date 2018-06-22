#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "../core/Ref.h"
#include "../core/Stream.h"

namespace gplay {

class Image;
class Uniform;
class BGFXTexture;


/**
 * Defines a standard texture.
 */
class Texture : public Ref
{
    friend class Sampler;
    friend class BGFXTexture;
    friend class FrameBuffer;

public:

    /**
     * Defines the set of supported texture formats.
     */
    enum Format
    {
        UNKNOWN = 0,
        RGBA16F,
        RGB,
        RGB888 = RGB,
        RGB565,
        RGBA,
        RGBA8888 = RGBA,
        RGBA4444,
        RGBA5551,
        ALPHA,
        D16,
        D24,
        D32,
        D16F,
        D24F,
        D32F,
    };

    /**
     * Defines the set of supported texture filters.
     */
    enum Filter
    {
        NEAREST,
        LINEAR,
        NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR
    };

    /**
     * Defines the set of supported texture wrapping modes.
     */
    enum Wrap
    {
        REPEAT,
        CLAMP,
        MIRROR,
        BORDER,
    };

    /**
     * Defines the type of Texture in use.
     */
    enum Type
    {
        TEXTURE_2D,
        TEXTURE_CUBE,
        TEXTURE_RT
    };

    /**
     * Defines a face of a Texture of Type: cube.
     */
    enum CubeFace
    {
        POSITIVE_X,
        NEGATIVE_X,
        POSITIVE_Y,
        NEGATIVE_Y,
        POSITIVE_Z,
        NEGATIVE_Z
    };

    /**
     * Texture information record
     */
    struct TextureInfo
    {
        unsigned int width;
        unsigned int height;
        Format format;
        unsigned int bytePerPixel;
        Type type;
        std::string id;
        uint32_t flags;
    };

    /**
     * Defines a texture sampler.
     *
     * A texture sampler is basically an instance of a texture that can be
     * used to sample a texture from a material. In addition to the texture
     * itself, a sampler stores per-instance texture state information, such
     * as wrap and filter modes.
     */
    class Sampler : public Ref
    {
        friend class Texture;

    public:

        /**
         * Destructor.
         */
        virtual ~Sampler();

        /**
         * Creates a sampler for the specified texture.
         *
         * @param texture The texture.
         *
         * @return The new sampler.
         * @script{create}
         */
        static Sampler* create(Texture* texture);

        /**
         * Creates a sampler for the specified texture.
         *
         * @param path Path to the texture to create a sampler for.
         * @param generateMipmaps True to force a full mipmap chain to be generated for the texture, false otherwise.
         *
         * @return The new sampler.
         * @script{create}
         */
        static Sampler* create(const char* path, bool generateMipmaps = false);

        /**
         * Sets the wrap mode for this sampler.
         *
         * @param wrapS The horizontal wrap mode.
         * @param wrapT The vertical wrap mode.
         * @param wrapR The depth wrap mode.
         */
        void setWrapMode(Wrap wrapS, Wrap wrapT, Wrap wrapR = REPEAT);

        /**
         * Sets the texture filter modes for this sampler.
         *
         * @param minificationFilter The texture minification filter.
         * @param magnificationFilter The texture magnification filter.
         */
        void setFilterMode(Filter minificationFilter, Filter magnificationFilter);

        /**
         * Specify custom flags for this sampler using the bgfx syntax.
         * see: BGFX_TEXTURE_ flags syntax.
         *
         * @param bgfxFlags The bgfx texture flags.
         */
        void setCustomFlags(unsigned int bgfxFlags);

        /**
         * Gets the texture for this sampler.
         *
         * @return The texture for this sampler.
         */
        Texture* getTexture() const;

        /**
         * Binds the texture of this sampler to the renderer and applies the sampler state.
         */
        void bind(Uniform *uniform);

    private:

        /**
         * Constructor.
         */
        Sampler(Texture* texture);

        /**
         * Hidden copy assignment operator.
         */
        Sampler& operator=(const Sampler&);

        Texture* _texture;
        Wrap _wrapS;
        Wrap _wrapT;
        Wrap _wrapR;
        Filter _minFilter;
        Filter _magFilter;
        unsigned int _bgfxFlags;
    };

    /**
     * Creates a texture from the given image resource.
     *
     * Note that for textures that include mipmap data in the source data (such as most compressed textures),
     * the generateMipmaps flags should NOT be set to true.
     *
     * @param path The image resource path.
     * @param generateMipmaps true to auto-generate a full mipmap chain, false otherwise.
     *
     * @return The new texture, or NULL if the texture could not be loaded/created.
     * @script{create}
     */
    static Texture* create(const char* path, bool generateMipmaps = false);

    /**
     * Creates a texture from the given image.
     *
     * @param image The image containing the texture data.
     * @param generateMipmaps True to generate a full mipmap chain, false otherwise.
     *
     * @return The new texture, or NULL if the image is not of a supported texture format.
     * @script{create}
     */
    static Texture* create(Image* image, bool generateMipmaps = false);

    /**
     * Creates a texture from the given texture data.
     *
     * The data in the texture is expected to be tightly packed (no padding at the end of rows).
     *
     * @param format Format of the texture data.
     * @param width Width of the texture data. If type is TEX_CUBE, then this is the cube face width.
     * @param height Height of the texture data. If type is TEX_CUBE, then this is the cube face height.
     * @param data Raw texture data (expected to be tightly packed). If the type parameter is set
     *   to TEXTURE_CUBE, then data is expected to be each face stored back contiguously within the
     *   array.
     * @param generateMipmaps True to generate a full mipmap chain, false otherwise.
     * @param type What type of Texture should be created.
     *
     * @return The new texture.
     * @script{create}
     */
    static Texture* create(Format format, unsigned int width, unsigned int height, const unsigned char* data, bool generateMipmaps = false, Type type = TEXTURE_2D, uint32_t flags = BGFX_TEXTURE_NONE);



    static Texture* create(TextureInfo& textureInfo);

    static Texture* create(const char* id, unsigned int width, unsigned int height, Format format, Type type = TEXTURE_2D, uint32_t flags = BGFX_TEXTURE_NONE);

    BGFXTexture* getGpuTexture() { return _gpuTtexture; }

    /**
     * Creates a texture object to wrap the specified pre-created native texture handle.
     *
     * The specified TextureHandle must represent a valid texture that has been created
     * on the underlying renderer and it should not be referenced by any other Texture
     * object. When the returned Texture object is destroyed, the passed in TextureHandle
     * will also be destroyed.
     *
     * @param handle Native texture handle.
     * @param width The width of the texture represented by 'handle'.
     * @param height The height of the texture represented by 'handle'.
     * @param format Optionally, the format of the texture represented by 'handle'.
     *      If the format cannot be represented by any of the Texture::Format values,
     *      use a value of UNKNOWN.
     *
     * @return The new texture.
     * @script{create}
     */
    static Texture* create(BGFXTexture *handle, int width, int height, Format format = UNKNOWN);

    /**
     * Returns the path that the texture was originally loaded from (if applicable).
     *
     * @return The texture path, or an empty string if the texture was not loaded from file.
     */
    const char* getPath() const;

    /**
     * Gets the format of the texture.
     *
     * @return The texture format.
     */
    Format getFormat() const;

    /**
     * Gets the texture type.
     *
     * @return The texture type.
     */
    Type getType() const;

    /**
     * Gets the texture width.
     *
     * @return The texture width.
     */
    unsigned int getWidth() const;

    /**
     * Gets the texture height.
     *
     * @return The texture height.
     */
    unsigned int getHeight() const;

    /**
     * Generates a full mipmap chain for this texture if it isn't already mipmapped.
     */
    void generateMipmaps();

    /**
     * Determines if this texture currently contains a full mipmap chain.
     *
     * @return True if this texture is currently mipmapped, false otherwise.
     */
    bool isMipmapped() const;

    /**
     * Determines if this texture is a compressed texture.
     */
    bool isCompressed() const;

    /**
     * Returns the texture handle.
     *
     * @return The texture handle.
     */
    BGFXTexture * getHandle() const;

private:

    /**
     * Constructor.
     */
    Texture();

    /**
     * Copy constructor.
     */
    Texture(const Texture& copy);

    /**
     * Destructor.
     */
    virtual ~Texture();

    /**
     * Hidden copy assignment operator.
     */
    Texture& operator=(const Texture&);


    static size_t getFormatBPP(Format format);

    std::string _path;
    BGFXTexture * _gpuTtexture;
    Format _format;
    Type _type;
    unsigned int _width;
    unsigned int _height;
    bool _mipmapped;
    bool _cached;
    bool _compressed;
    Wrap _wrapS;
    Wrap _wrapT;
    Wrap _wrapR;
    Filter _minFilter;
    Filter _magFilter;
    size_t _bpp;
};

}

#endif
