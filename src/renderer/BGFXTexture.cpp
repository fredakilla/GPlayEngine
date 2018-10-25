#include "../renderer/BGFXTexture.h"
#include "../graphics/Texture.h"
#include "../renderer/BGFXUniform.h"
#include "../core/FileSystem.h"

#ifdef GP_USE_MEM_LEAK_DETECTION
#undef new
    #include <bx/allocator.h>
#define new DEBUG_NEW
#else
    #include <bx/allocator.h>
#endif
#include <bimg/decode.h>


namespace gplay {

/**
----------------------------------------------------------------------
  GP3D <=> BGFX - texture format
----------------------------------------------------------------------
    Texture::Format::UNKNOWN   == bgfx::TextureFormat::Unknown
    Texture::Format::RGB       == bgfx::TextureFormat::RGB8
    Texture::Format::RGB888    == bgfx::TextureFormat::RGB8
    Texture::Format::RGB565    == bgfx::TextureFormat::R5G6B5
    Texture::Format::RGBA      == bgfx::TextureFormat::RGBA8
    Texture::Format::RGBA8888  == bgfx::TextureFormat::RGBA8
    Texture::Format::RGBA4444  == bgfx::TextureFormat::RGBA4
    Texture::Format::RGBA5551  == bgfx::TextureFormat::RGB5A1
    Texture::Format::ALPHA     == bgfx::TextureFormat::R8
    Texture::Format::D16       == bgfx::TextureFormat::D16
    Texture::Format::D32       == bgfx::TextureFormat::D32
----------------------------------------------------------------------
*/

bgfx::TextureFormat::Enum BGFXTexture::toBgfxFormat(Texture::Format gp3dFormat)
{
    switch(gp3dFormat)
    {
    case Texture::Format::UNKNOWN    : return bgfx::TextureFormat::Unknown;
    case Texture::Format::RGB        : return bgfx::TextureFormat::RGB8;
    case Texture::Format::RGB565     : return bgfx::TextureFormat::R5G6B5;
    case Texture::Format::RGBA       : return bgfx::TextureFormat::RGBA8;
    case Texture::Format::RGBA4444   : return bgfx::TextureFormat::RGBA4;
    case Texture::Format::RGBA5551   : return bgfx::TextureFormat::RGB5A1;
    case Texture::Format::RGBA16F    : return bgfx::TextureFormat::RGBA16F;
    case Texture::Format::ALPHA      : return bgfx::TextureFormat::R8;
    case Texture::Format::D16        : return bgfx::TextureFormat::D16;
    case Texture::Format::D24        : return bgfx::TextureFormat::D24;
    case Texture::Format::D32        : return bgfx::TextureFormat::D32;
    case Texture::Format::D16F        : return bgfx::TextureFormat::D16F;
    case Texture::Format::D24F        : return bgfx::TextureFormat::D24F;
    case Texture::Format::D32F        : return bgfx::TextureFormat::D32F;
    default:
        GP_ASSERT(!"gp3d texture format unknown.");
        return bgfx::TextureFormat::Unknown;
    }
}

Texture::Format BGFXTexture::toGp3dFormat(bgfx::TextureFormat::Enum bimgTextureFormat)
{
    switch(bimgTextureFormat)
    {
    case bgfx::TextureFormat::Unknown   : return Texture::Format::UNKNOWN;
    case bgfx::TextureFormat::RGB8      : return Texture::Format::RGB;
    case bgfx::TextureFormat::R5G6B5    : return Texture::Format::RGB565;
    case bgfx::TextureFormat::RGBA8     : return Texture::Format::RGBA;
    case bgfx::TextureFormat::RGBA4     : return Texture::Format::RGBA4444;
    case bgfx::TextureFormat::RGB5A1    : return Texture::Format::RGBA5551;
    case bgfx::TextureFormat::RGBA16F   : return Texture::Format::RGBA16F;
    case bgfx::TextureFormat::R8        : return Texture::Format::ALPHA;
    case bgfx::TextureFormat::D16       : return Texture::Format::D16;
    case bgfx::TextureFormat::D24       : return Texture::Format::D24;
    case bgfx::TextureFormat::D32       : return Texture::Format::D32;
    case bgfx::TextureFormat::D16F      : return Texture::Format::D16F;
    case bgfx::TextureFormat::D24F      : return Texture::Format::D24F;
    case bgfx::TextureFormat::D32F      : return Texture::Format::D32F;

    case bgfx::TextureFormat::BC2       : return Texture::Format::RGBA;
    case bgfx::TextureFormat::BC3       : return Texture::Format::RGBA;
    case bgfx::TextureFormat::BC5       : return Texture::Format::RGBA;

    default:
        GP_ASSERT(!"bimg texture format not supported.");
        return Texture::Format::UNKNOWN;
    }
}

uint32_t MIN_FILTER[] =
{
    BGFX_SAMPLER_MIN_POINT,         //NEAREST = GL_NEAREST,
    BGFX_SAMPLER_MIN_ANISOTROPIC,   //LINEAR = GL_LINEAR,
    BGFX_SAMPLER_MIP_POINT,         //NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    BGFX_SAMPLER_MIP_POINT,         //LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
    BGFX_TEXTURE_NONE,              //NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
    BGFX_TEXTURE_NONE,              //LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
};

uint32_t MAG_FILTER[] =
{
    BGFX_SAMPLER_MAG_POINT,         //NEAREST = GL_NEAREST,
    BGFX_SAMPLER_MAG_ANISOTROPIC,   //LINEAR = GL_LINEAR,
    BGFX_SAMPLER_MIP_POINT,         //NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    BGFX_SAMPLER_MIP_POINT,         //LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
    BGFX_TEXTURE_NONE,              //NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
    BGFX_TEXTURE_NONE,              //LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
};

uint32_t WRAP_S[] =
{
    BGFX_SAMPLER_NONE,              //REPEAT = GL_REPEAT,
    BGFX_SAMPLER_U_CLAMP,           //CLAMP = GL_CLAMP_TO_EDGE
    BGFX_SAMPLER_U_MIRROR,
    BGFX_SAMPLER_U_BORDER,
};

uint32_t WRAP_T[] =
{
    BGFX_SAMPLER_NONE,              //REPEAT = GL_REPEAT,
    BGFX_SAMPLER_V_CLAMP,           //CLAMP = GL_CLAMP_TO_EDGE
    BGFX_SAMPLER_V_MIRROR,
    BGFX_SAMPLER_V_BORDER,
};

uint32_t WRAP_R[] =
{
    BGFX_SAMPLER_NONE,              //REPEAT = GL_REPEAT,
    BGFX_SAMPLER_W_CLAMP,           //CLAMP = GL_CLAMP_TO_EDGE
    BGFX_SAMPLER_W_MIRROR,
    BGFX_SAMPLER_W_BORDER,
};



bx::AllocatorI* getDefaultAllocator()
{
    static bx::DefaultAllocator s_allocator;
    return &s_allocator;
}

bgfx::TextureHandle createTexture(bimg::ImageContainer* imageContainer, uint32_t flags, bgfx::TextureInfo* info = NULL)
{
    bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

    const bgfx::Memory* mem = NULL;
    if(imageContainer->m_data)
    {
        mem = bgfx::makeRef(
                    imageContainer->m_data
                    , imageContainer->m_size
                    , 0
                    , imageContainer
                    );
    }

    if (imageContainer->m_cubeMap)
    {
        handle = bgfx::createTextureCube(
                    uint16_t(imageContainer->m_width)
                    , 1 < imageContainer->m_numMips
                    , imageContainer->m_numLayers
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)
                    , flags
                    , mem
                    );
    }
    /*else if (1 < imageContainer->m_depth)
    {
        handle = bgfx::createTexture3D(
                    uint16_t(imageContainer->m_width)
                    , uint16_t(imageContainer->m_height)
                    , uint16_t(imageContainer->m_depth)
                    , 1 < imageContainer->m_numMips
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)
                    , flags
                    , mem
                    );
    }*/
    else
    {
        handle = bgfx::createTexture2D(
                    uint16_t(imageContainer->m_width)
                    , uint16_t(imageContainer->m_height)
                    , 1 < imageContainer->m_numMips
                    , imageContainer->m_numLayers
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)
                    , flags
                    , mem
                    );
    }

    if (NULL != info)
    {
        bgfx::calcTextureSize(
                    *info
                    , uint16_t(imageContainer->m_width)
                    , uint16_t(imageContainer->m_height)
                    , uint16_t(imageContainer->m_depth)
                    , imageContainer->m_cubeMap
                    , 1 < imageContainer->m_numMips
                    , imageContainer->m_numLayers
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)
                    );
    }

    return handle;
}

bgfx::TextureHandle loadTexture(const char* filePath, uint32_t flags, bgfx::TextureInfo* info = NULL, bimg::Orientation::Enum* orientation = NULL)
{
    // Read file
    int size = 0;
    char * data = FileSystem::readAll(filePath, &size);
    if (data == NULL)
    {
        GP_ERROR("Failed to read image from file '%s'.", filePath);
        return BGFX_INVALID_HANDLE;
    }

    // Parse data
    bimg::ImageContainer* imageContainer = bimg::imageParse(getDefaultAllocator(), data, size);
    if(imageContainer == nullptr)
    {
        GP_ERROR("Failed to parse image data from file '%s'.", filePath);
        return BGFX_INVALID_HANDLE;
    }

    // create bgfx texture
    bgfx::TextureHandle handle = createTexture(imageContainer, flags, info);
    GP_ASSERT(bgfx::isValid(handle));

    // free file data
    delete[] data;

    return handle;
}



BGFXTexture::BGFXTexture() :
    _handle(BGFX_INVALID_HANDLE)
{
}

BGFXTexture::~BGFXTexture()
{
    if(bgfx::isValid(_handle))
        bgfx::destroy(_handle);
}

Texture * BGFXTexture::createFromFile(const char * path)
{
    BGFXTexture * bgfxTexture = new BGFXTexture();

    // create texture from file
    bgfx::TextureInfo info;
    bgfxTexture->_handle = loadTexture(path, BGFX_TEXTURE_NONE, &info);

    // create gameplay3d texture
    Texture* texture = new Texture();
    texture->_format = BGFXTexture::toGp3dFormat(info.format);
    texture->_type = Texture::TEXTURE_2D;
    texture->_width = info.width;
    texture->_height = info.height;
    texture->_compressed = false;
    texture->_mipmapped = info.numMips > 1;
    texture->_bpp = info.bitsPerPixel / 8;
    texture->_path = path;
    texture->_gpuTtexture = bgfxTexture;

    return texture;
}

Texture* BGFXTexture::createFromData(Texture::TextureInfo info, const unsigned char* data, uint32_t flags)
{
    BGFXTexture * bgfxTexture = new BGFXTexture();

    unsigned int width = info.width;
    unsigned int height = info.height;
    uint32_t imgSize = width * height * info.bytePerPixel;
    bimg::TextureFormat::Enum bgfxFormat = (bimg::TextureFormat::Enum)toBgfxFormat(info.format);
    //uint32_t imgSize = bimg::imageGetSize(0, width, height, 1, false, false, 1, bgfxFormat);

    bimg::ImageContainer * imageContainer = new bimg::ImageContainer();
    imageContainer->m_size = imgSize;
    imageContainer->m_offset = 0;
    imageContainer->m_width = width;
    imageContainer->m_height = height;
    imageContainer->m_depth = 1;
    imageContainer->m_numLayers = 1;
    imageContainer->m_numMips = 1;
    imageContainer->m_hasAlpha = info.bytePerPixel > 3 ? true : false;
    imageContainer->m_cubeMap = false;
    imageContainer->m_ktx = false;
    imageContainer->m_ktxLE = false;
    imageContainer->m_srgb = false;
    imageContainer->m_format = bgfxFormat;
    imageContainer->m_orientation = bimg::Orientation::R0;

    if(data)
    {
        const bgfx::Memory* mem = bgfx::copy(data, imgSize);
        imageContainer->m_data = mem->data;
    }

    flags |= info.flags;
    if(info.type == Texture::TEXTURE_RT)
    {
        flags |= BGFX_TEXTURE_RT;
    }

    bgfx::TextureInfo bgfxInfo;
    bgfxTexture->_handle = createTexture(imageContainer, flags, &bgfxInfo);

    // create gameplay3d texture
    Texture* texture = new Texture();
    texture->_format = BGFXTexture::toGp3dFormat(bgfxInfo.format);
    texture->_type = info.type;
    texture->_width = bgfxInfo.width;
    texture->_height = bgfxInfo.height;
    texture->_compressed = false;
    texture->_mipmapped = bgfxInfo.numMips > 1;
    texture->_bpp = bgfxInfo.bitsPerPixel / 8;
    texture->_gpuTtexture = bgfxTexture;
    texture->_path = info.id;

    return texture;
}

void BGFXTexture::bind(Uniform * uniform, Texture * texture, uint32_t customFlags)
{
    // set texture flags using gplay filter and wrap modes
    uint32_t flags = BGFX_TEXTURE_NONE
            | MIN_FILTER[texture->_minFilter]
            | MAG_FILTER[texture->_magFilter]
            | WRAP_S[texture->_wrapS]
            | WRAP_T[texture->_wrapT]
            | WRAP_R[texture->_wrapR];

    // add custom flags, if specified. (note: filter and wrap can be directly specified using the customFlags)
    flags |= customFlags;

    BGFXUniform * bgfxUniform = static_cast<BGFXUniform*>(uniform);
    bgfx::setTexture(bgfxUniform->getIndex(), bgfxUniform->getHandle(), _handle, flags);
}

} // end namespace gplay
