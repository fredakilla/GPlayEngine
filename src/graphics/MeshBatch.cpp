#include "../core/Base.h"
#include "../graphics/MeshBatch.h"
#include "../graphics/Material.h"
#include "../renderer/BGFXVertexBuffer.h"

namespace gplay {

MeshBatch::MeshBatch(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, Material* material, bool indexed, unsigned int initialCapacity, unsigned int growSize)
    : _vertexFormat(vertexFormat), _primitiveType(primitiveType), _material(material), _indexed(indexed), _capacity(0), _growSize(growSize),
    _vertexCapacity(0), _indexCapacity(0), _vertexCount(0), _indexCount(0), _vertices(NULL), _verticesPtr(NULL), _indices(NULL), _indicesPtr(NULL), _started(false)
{
    BGFXVertexBuffer::createVertexDecl(vertexFormat, _vertexDecl);

    resize(initialCapacity);
}

MeshBatch::~MeshBatch()
{
    SAFE_RELEASE(_material);
    SAFE_DELETE_ARRAY(_vertices);
    SAFE_DELETE_ARRAY(_indices);
}

MeshBatch* MeshBatch::create(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, const char* materialPath, bool indexed, unsigned int initialCapacity, unsigned int growSize)
{
    Material* material = Material::create(materialPath);
    if (material == NULL)
    {
        GP_ERROR("Failed to create material for mesh batch from file '%s'.", materialPath);
        return NULL;
    }
    MeshBatch* batch = create(vertexFormat, primitiveType, material, indexed, initialCapacity, growSize);
    SAFE_RELEASE(material); // batch now owns the material
    return batch;
}

MeshBatch* MeshBatch::create(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, Material* material, bool indexed, unsigned int initialCapacity, unsigned int growSize)
{
    GP_ASSERT(material);

    MeshBatch* batch = new MeshBatch(vertexFormat, primitiveType, material, indexed, initialCapacity, growSize);

    material->addRef();

    return batch;
}

void MeshBatch::add(const void* vertices, size_t size, unsigned int vertexCount, const unsigned short* indices, unsigned int indexCount)
{
    GP_ASSERT(vertices);
    
    unsigned int newVertexCount = _vertexCount + vertexCount;
    unsigned int newIndexCount = _indexCount + indexCount;
    if (_primitiveType == Mesh::TRIANGLE_STRIP && _vertexCount > 0)
        newIndexCount += 2; // need an extra 2 indices for connecting strips with degenerate triangles
    
    // Do we need to grow the batch?
    while (newVertexCount > _vertexCapacity || (_indexed && newIndexCount > _indexCapacity))
    {
        if (_growSize == 0)
            return; // growing disabled, just clip batch
        if (!resize(_capacity + _growSize))
            return; // failed to grow
    }
    
    // Copy vertex data.
    GP_ASSERT(_verticesPtr);
    unsigned int vBytes = vertexCount * _vertexFormat.getVertexSize();
    memcpy(_verticesPtr, vertices, vBytes);
    
    // Copy index data.
    if (_indexed)
    {
        GP_ASSERT(indices);
        GP_ASSERT(_indicesPtr);

        if (_vertexCount == 0)
        {
            // Simply copy values directly into the start of the index array.
            memcpy(_indicesPtr, indices, indexCount * sizeof(unsigned short));
        }
        else
        {
            if (_primitiveType == Mesh::TRIANGLE_STRIP)
            {
                // Create a degenerate triangle to connect separate triangle strips
                // by duplicating the previous and next vertices.
                _indicesPtr[0] = *(_indicesPtr-1);
                _indicesPtr[1] = _vertexCount;
                _indicesPtr += 2;
            }
            
            // Loop through all indices and insert them, with their values offset by
            // 'vertexCount' so that they are relative to the first newly inserted vertex.
            for (unsigned int i = 0; i < indexCount; ++i)
            {
                _indicesPtr[i] = indices[i] + _vertexCount;
            }
        }
        _indicesPtr += indexCount;
        _indexCount = newIndexCount;
    }
    
    _verticesPtr += vBytes;
    _vertexCount = newVertexCount;
}

unsigned int MeshBatch::getCapacity() const
{
    return _capacity;
}

void MeshBatch::setCapacity(unsigned int capacity)
{
    resize(capacity);
}

bool MeshBatch::resize(unsigned int capacity)
{
    if (capacity == 0)
    {
        GP_ERROR("Invalid resize capacity (0).");
        return false;
    }

    if (capacity == _capacity)
        return true;

    // Store old batch data.
    unsigned char* oldVertices = _vertices;
    unsigned short* oldIndices = _indices;

    unsigned int vertexCapacity = 0;
    switch (_primitiveType)
    {
    case Mesh::LINES:
        vertexCapacity = capacity * 2;
        break;
    case Mesh::LINE_STRIP:
        vertexCapacity = capacity + 1;
        break;
    case Mesh::POINTS:
        vertexCapacity = capacity;
        break;
    case Mesh::TRIANGLES:
        vertexCapacity = capacity * 3;
        break;
    case Mesh::TRIANGLE_STRIP:
        vertexCapacity = capacity + 2;
        break;
    default:
        GP_ERROR("Unsupported primitive type for mesh batch (%d).", _primitiveType);
        return false;
    }

    // We have no way of knowing how many vertices will be stored in the batch
    // (we only know how many indices will be stored). Assume the worst case
    // for now, which is the same number of vertices as indices.
    unsigned int indexCapacity = vertexCapacity;
    if (_indexed && indexCapacity > USHRT_MAX)
    {
        GP_ERROR("Index capacity is greater than the maximum unsigned short value (%d > %d).", indexCapacity, USHRT_MAX);
        return false;
    }

    // Allocate new data and reset pointers.
    unsigned int voffset = _verticesPtr - _vertices;
    unsigned int vBytes = vertexCapacity * _vertexFormat.getVertexSize();
    _vertices = new unsigned char[vBytes];
    if (voffset >= vBytes)
        voffset = vBytes - 1;
    _verticesPtr = _vertices + voffset;

    if (_indexed)
    {
        unsigned int ioffset = _indicesPtr - _indices;
        _indices = new unsigned short[indexCapacity];
        if (ioffset >= indexCapacity)
            ioffset = indexCapacity - 1;
        _indicesPtr = _indices + ioffset;
    }

    // Copy old data back in
    if (oldVertices)
        memcpy(_vertices, oldVertices, std::min(_vertexCapacity, vertexCapacity) * _vertexFormat.getVertexSize());
    SAFE_DELETE_ARRAY(oldVertices);
    if (oldIndices)
        memcpy(_indices, oldIndices, std::min(_indexCapacity, indexCapacity) * sizeof(unsigned short));
    SAFE_DELETE_ARRAY(oldIndices);

    // Assign new capacities
    _capacity = capacity;
    _vertexCapacity = vertexCapacity;
    _indexCapacity = indexCapacity;

    return true;
}

void MeshBatch::add(const float* vertices, unsigned int vertexCount, const unsigned short* indices, unsigned int indexCount)
{
    add(vertices, sizeof(float), vertexCount, indices, indexCount);
}

void MeshBatch::start()
{
    _vertexCount = 0;
    _indexCount = 0;
    _verticesPtr = _vertices;
    _indicesPtr = _indices;
    _started = true;
}

bool MeshBatch::isStarted() const
{
    return _started;
}

void MeshBatch::finish()
{
    _started = false;
}

void MeshBatch::draw()
{
    if (_vertexCount == 0 || (_indexed && _indexCount == 0))
        return; // nothing to draw

    // using bgfx transient buffers

    uint32_t maxVertices = bgfx::getAvailTransientVertexBuffer(_vertexCount, _vertexDecl);
    if(maxVertices < _vertexCount)
        GP_WARN("Available transient vertices count is less than requested %d/%d.", _vertexCount, maxVertices);

    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(&tvb, maxVertices, _vertexDecl);
    memcpy(tvb.data, &_vertices[0], _vertexDecl.getSize(maxVertices));
    bgfx::setVertexBuffer(0, &tvb, 0, maxVertices);

    if(_indexed)
    {
        uint32_t maxIndices = bgfx::getAvailTransientIndexBuffer(_indexCount);
        if(maxIndices < _indexCount)
            GP_WARN("Available transient indices count is less than requested %d/%d.", _indexCount, maxIndices);

        bgfx::TransientIndexBuffer tib;
        bgfx::allocTransientIndexBuffer(&tib, maxIndices);
        memcpy(tib.data, &_indices[0], sizeof(unsigned short)*maxIndices);
        bgfx::setIndexBuffer(&tib, 0, maxIndices);
    }

    // Bind the material.
    Technique* technique = _material->getTechnique();
    GP_ASSERT(technique);
    unsigned int passCount = technique->getPassCount();
    for (unsigned int i = 0; i < passCount; ++i)
    {
        Pass* pass = technique->getPassByIndex(i);
        GP_ASSERT(pass);
        pass->bind(_primitiveType);
        pass->unbind();
    }

    return;
}    

}
