#include "../core/Base.h"
#include "../graphics/Image.h"
#include "../graphics/Texture.h"
#include "../core/FileSystem.h"
#include "../renderer/BGFXTexture.h"

namespace gplay {

static std::vector<Texture*> __textureCache;

Texture::Texture() :
    _gpuTtexture(nullptr),
    _format(UNKNOWN),
    _type((Texture::Type)0),
    _width(0),
    _height(0),
    _mipmapped(false),
    _cached(false),
    _compressed(false),
    _wrapS(Texture::REPEAT),
    _wrapT(Texture::REPEAT),
    _wrapR(Texture::REPEAT),
    _minFilter(Texture::NEAREST_MIPMAP_LINEAR),
    _magFilter(Texture::LINEAR)
{
}

Texture::~Texture()
{
    if (_gpuTtexture)
    {
        SAFE_DELETE(_gpuTtexture);
    }

    // Remove ourself from the texture cache.
    if (_cached)
    {
        std::vector<Texture*>::iterator itr = std::find(__textureCache.begin(), __textureCache.end(), this);
        if (itr != __textureCache.end())
        {
            __textureCache.erase(itr);
        }
    }
}

Texture* Texture::create(const char* path, bool generateMipmaps)
{
    GP_ASSERT( path );

    // Search texture cache first.
    for (size_t i = 0, count = __textureCache.size(); i < count; ++i)
    {
        Texture* t = __textureCache[i];
        GP_ASSERT( t );
        if (t->_path == path)
        {
            // If 'generateMipmaps' is true, call Texture::generateMipamps() to force the
            // texture to generate its mipmap chain if it hasn't already done so.
            if (generateMipmaps)
            {
                t->generateMipmaps();
            }

            // Found a match.
            t->addRef();

            return t;
        }
    }

    // Create texture.
    Texture* texture = BGFXTexture::createFromFile(path);

    if (texture)
    {
        GP_ASSERT(texture->_gpuTtexture);

        texture->_path = path;
        texture->_cached = true;

        // Add to texture cache.
        __textureCache.push_back(texture);

        return texture;
    }

    GP_ERROR("Failed to load texture from file '%s'.", path);
    return NULL;
}

Texture* Texture::create(Image* image, bool generateMipmaps)
{
    GP_ASSERT( image );

    switch (image->getFormat())
    {
    case Image::RGB:
        return create(Texture::RGB, image->getWidth(), image->getHeight(), image->getData(), generateMipmaps);
    case Image::RGBA:
        return create(Texture::RGBA, image->getWidth(), image->getHeight(), image->getData(), generateMipmaps);
    default:
        GP_ERROR("Unsupported image format (%d).", image->getFormat());
        return NULL;
    }
}

size_t Texture::getFormatBPP(Format format)
{
    switch (format)
    {
    case Texture::RGB565:
    case Texture::RGBA4444:
    case Texture::RGBA5551:
        return 2;
    case Texture::RGB888:
        return 3;
    case Texture::RGBA8888:
        return 4;
    case Texture::ALPHA:
        return 1;
    default:
        return 0;
    }
}

Texture* Texture::create(Format format, unsigned int width, unsigned int height, const unsigned char* data,
                         bool generateMipmaps, Texture::Type type, uint32_t flags)
{
    TextureInfo textureInfo;
    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.format = format;
    textureInfo.bytePerPixel = getFormatBPP(format);
    textureInfo.type = type;
    textureInfo.id = "";
    textureInfo.flags = 0;

    Texture* texture = BGFXTexture::createFromData(textureInfo, data);
    if (generateMipmaps)
        texture->generateMipmaps();

    return texture;
}

Texture* Texture::create(TextureInfo& textureInfo)
{
    textureInfo.bytePerPixel = getFormatBPP(textureInfo.format);
    return BGFXTexture::createFromData(textureInfo);
}

Texture* Texture::create(const char* id, unsigned int width, unsigned int height, Format format, Type type, uint32_t flags)
{
    TextureInfo textureInfo;
    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.format = format;
    textureInfo.bytePerPixel = getFormatBPP(format);
    textureInfo.type = type;
    textureInfo.id = id;
    textureInfo.flags = flags;

    Texture* texture = BGFXTexture::createFromData(textureInfo);

    return texture;
}

Texture* Texture::create(BGFXTexture* handle, int width, int height, Format format)
{
    GP_ASSERT( handle );

    Texture* texture = new Texture();
    texture->_gpuTtexture = handle;
    texture->_format = format;
    texture->_width = width;
    texture->_height = height;
    texture->_bpp = getFormatBPP(format);

    return texture;
}

Texture::Format Texture::getFormat() const
{
    return _format;
}

Texture::Type Texture::getType() const
{
    return _type;
}

const char* Texture::getPath() const
{
    return _path.c_str();
}

unsigned int Texture::getWidth() const
{
    return _width;
}

unsigned int Texture::getHeight() const
{
    return _height;
}

BGFXTexture *Texture::getHandle() const
{
    return _gpuTtexture;
}

void Texture::generateMipmaps()
{
    if (!_mipmapped)
    {
        _mipmapped = true;
    }

    //@@if (!_mipmapped)
    //@@{
    //@@    GLenum target = (GLenum)_type;
    //@@    GL_ASSERT( glBindTexture(target, _handle) );
    //@@    GL_ASSERT( glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST) );
    //@@    if( std::addressof(glGenerateMipmap) )
    //@@        GL_ASSERT( glGenerateMipmap(target) );
    //@@
    //@@    _mipmapped = true;
    //@@
    //@@    // Restore the texture id
    //@@    GL_ASSERT( glBindTexture((GLenum)__currentTextureType, __currentTextureId) );
    //@@}
}

bool Texture::isMipmapped() const
{
    return _mipmapped;
}

bool Texture::isCompressed() const
{
    return _compressed;
}


//-------------------------------------------------------------------------
//
// Texture Sampler
//
//-------------------------------------------------------------------------

Texture::Sampler::Sampler(Texture* texture)
    : _texture(texture),
      _wrapS(Texture::REPEAT),
      _wrapT(Texture::REPEAT),
      _wrapR(Texture::REPEAT),
      _bgfxFlags(0)
{
    GP_ASSERT( texture );
    _minFilter = texture->_minFilter;
    _magFilter = texture->_magFilter;
}

Texture::Sampler::~Sampler()
{
    SAFE_RELEASE(_texture);
}

Texture::Sampler* Texture::Sampler::create(Texture* texture)
{
    GP_ASSERT( texture );
    GP_ASSERT( texture->_type == Texture::TEXTURE_2D || texture->_type == Texture::TEXTURE_CUBE || texture->_type == Texture::TEXTURE_RT);
    texture->addRef();
    return new Sampler(texture);
}

Texture::Sampler* Texture::Sampler::create(const char* path, bool generateMipmaps)
{
    Texture* texture = Texture::create(path, generateMipmaps);
    return texture ? new Sampler(texture) : NULL;
}

void Texture::Sampler::setWrapMode(Wrap wrapS, Wrap wrapT, Wrap wrapR)
{
    _wrapS = wrapS;
    _wrapT = wrapT;
    _wrapR = wrapR;
}

void Texture::Sampler::setFilterMode(Filter minificationFilter, Filter magnificationFilter)
{
    _minFilter = minificationFilter;
    _magFilter = magnificationFilter;
}

void Texture::Sampler::setCustomFlags(unsigned int bgfxFlags)
{
    _bgfxFlags = bgfxFlags;
}

Texture* Texture::Sampler::getTexture() const
{
    return _texture;
}

void Texture::Sampler::bind(Uniform * uniform)
{
    GP_ASSERT( _texture );

    if (_texture->_minFilter != _minFilter)
    {
        _texture->_minFilter = _minFilter;
    }
    if (_texture->_magFilter != _magFilter)
    {
        _texture->_magFilter = _magFilter;
    }
    if (_texture->_wrapS != _wrapS)
    {
        _texture->_wrapS = _wrapS;
    }
    if (_texture->_wrapT != _wrapT)
    {
        _texture->_wrapT = _wrapT;
    }
    if (_texture->_wrapR != _wrapR)
    {
        _texture->_wrapR = _wrapR;
    }

    _texture->_gpuTtexture->bind(uniform, _texture, _bgfxFlags);
}

}
