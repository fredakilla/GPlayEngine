#include "../renderer/BGFXVertexBuffer.h"

namespace gplay {

BGFXVertexBuffer::BGFXVertexBuffer(const VertexFormat &vertexFormat, uint32_t vertexCount, bool dynamic) :
    _svbh(BGFX_INVALID_HANDLE)
  , _dvbh(BGFX_INVALID_HANDLE)
{
    // create vertex declaration
    createVertexDecl(vertexFormat, _vertexDecl);

    // initialise Geometry buffer.
    initialize(_vertexDecl.getSize(1), vertexCount, dynamic);

    // if dynamic, create bgfx vertex buffer here.
    // if static, creation will be delayed when setting vertice data.
    if(_dynamic)
    {
        createDynamicBuffer();
    }
}

BGFXVertexBuffer::~BGFXVertexBuffer()
{
    if(_dynamic)
    {
        if(bgfx::isValid(_dvbh))
            bgfx::destroy(_dvbh);
    }
    else
    {
        if(bgfx::isValid(_svbh))
            bgfx::destroy(_svbh);
    }
}

void getBgfxAttribute(const VertexFormat::Element& element, bgfx::Attrib::Enum& attrib)
{
    switch(element.usage)
    {
        case VertexFormat::POSITION:        attrib = bgfx::Attrib::Position;    break;
        case VertexFormat::NORMAL:          attrib = bgfx::Attrib::Normal;      break;
        case VertexFormat::COLOR:           attrib = bgfx::Attrib::Color0;      break;
        case VertexFormat::TANGENT:         attrib = bgfx::Attrib::Tangent;     break;
        case VertexFormat::BINORMAL:        attrib = bgfx::Attrib::Bitangent;   break;
        case VertexFormat::BLENDWEIGHTS:    attrib = bgfx::Attrib::Weight;      break;
        case VertexFormat::BLENDINDICES:    attrib = bgfx::Attrib::Indices;     break;
        case VertexFormat::TEXCOORD0:       attrib = bgfx::Attrib::TexCoord0;   break;
        case VertexFormat::TEXCOORD1:       attrib = bgfx::Attrib::TexCoord1;   break;
        case VertexFormat::TEXCOORD2:       attrib = bgfx::Attrib::TexCoord2;   break;
        case VertexFormat::TEXCOORD3:       attrib = bgfx::Attrib::TexCoord3;   break;
        case VertexFormat::TEXCOORD4:       attrib = bgfx::Attrib::TexCoord4;   break;
        case VertexFormat::TEXCOORD5:       attrib = bgfx::Attrib::TexCoord5;   break;
        case VertexFormat::TEXCOORD6:       attrib = bgfx::Attrib::TexCoord6;   break;
        case VertexFormat::TEXCOORD7:       attrib = bgfx::Attrib::TexCoord7;   break;
        default:                            attrib = bgfx::Attrib::Count;
    }
}

bgfx::AttribType::Enum getBgfxAttributeType(const VertexFormat::AttribType type)
{
    switch (type)
    {
    case VertexFormat::Uint8:
        return bgfx::AttribType::Uint8;
        break;
    case VertexFormat::Int16:
        return bgfx::AttribType::Int16;
        break;
    case VertexFormat::Float:
        return bgfx::AttribType::Float;
        break;
    default:
        GP_ERROR("Attribute type unknown.");
        break;
    }
}

void BGFXVertexBuffer::createVertexDecl(const VertexFormat &vertexFormat, bgfx::VertexDecl& vertexDecl)
{
    vertexDecl.begin();

    for(size_t i=0; i<vertexFormat.getElementCount(); ++i)
    {
        const VertexFormat::Element element = vertexFormat.getElement(i);

        bgfx::Attrib::Enum attrib;
        getBgfxAttribute(element, attrib);

        bgfx::AttribType::Enum type = getBgfxAttributeType(element.type);
        bool normalized = element.normalized;
        uint8_t num = element.size;

        bool asInt = element.type == VertexFormat::Float ? false : true;

        vertexDecl.add(attrib,num,type,normalized,asInt);
    }

    vertexDecl.end();
}

void BGFXVertexBuffer::createStaticBuffer()
{
    GP_ASSERT(!_dynamic && !bgfx::isValid(_svbh));
    GP_ASSERT(_memoryBuffer.getSize() > 0);

    void* dataPtr = _memoryBuffer.map(0);
    GP_ASSERT(dataPtr);
    const bgfx::Memory * mem = bgfx::makeRef(dataPtr, _memoryBuffer.getSize());

    uint16_t flags = BGFX_BUFFER_NONE;
    _svbh = bgfx::createVertexBuffer(mem, _vertexDecl, flags);
    GP_ASSERT(bgfx::isValid(_svbh));
}

void BGFXVertexBuffer::createDynamicBuffer()
{
    GP_ASSERT(_dynamic);

    uint16_t flags = /*BGFX_BUFFER_NONE; //*/BGFX_BUFFER_ALLOW_RESIZE;
    _dvbh = bgfx::createDynamicVertexBuffer(_elementCount, _vertexDecl, flags);
    GP_ASSERT(bgfx::isValid(_dvbh));
}

void BGFXVertexBuffer::set(const void* data, uint32_t count, uint32_t start)
{
    GeometryBuffer::set(data, count, start);

    if(_dynamic)
    {
        // if dynamic, update vertex buffer with memory buffer
        GP_ASSERT(bgfx::isValid(_dvbh));
        const bgfx::Memory* mem = bgfx::makeRef(_memoryBuffer.map(0), _memoryBuffer.getSize());
        bgfx::update(_dvbh, 0, mem);
    }
    else
    {
        // create static bgfx buffer
        if(!bgfx::isValid(_svbh))
            createStaticBuffer();
    }
}

void BGFXVertexBuffer::bind() const
{
    if(_dynamic)
    {
        GP_ASSERT(bgfx::isValid(_dvbh));
        bgfx::setVertexBuffer(0, _dvbh, _drawStart, _drawCount);
    }
    else
    {
        GP_ASSERT(bgfx::isValid(_svbh));
        bgfx::setVertexBuffer(0, _svbh, _drawStart, _drawCount);
    }
}

void * BGFXVertexBuffer::lock(uint32_t start, uint32_t count)
{
    return GeometryBuffer::lock(start, count);
}

void BGFXVertexBuffer::unLock()
{
    GP_ASSERT(bgfx::isValid(_dvbh));

    if (_lockState == LOCK_ACTIVE)
    {
        bgfx::update(_dvbh, _lockStart, bgfx::makeRef(_lockData, _lockCount * _elementSize));
        GeometryBuffer::unLock();
    }    
}

} // end namespace gplay
