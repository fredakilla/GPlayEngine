#include "../core/Base.h"
#include "../graphics/MeshPart.h"
#include "../renderer/BGFXIndexBuffer.h"

namespace gplay
{

MeshPart::MeshPart() :
    _mesh(NULL), _meshIndex(0), _primitiveType(Mesh::TRIANGLES), _indexCount(0), _indexBuffer(0), _dynamic(false)
{
}

MeshPart::~MeshPart()
{
    SAFE_DELETE(_indexBuffer);
}

void MeshPart::set(Mesh::IndexFormat indexFormat, unsigned int indexCount, bool dynamic)
{
    _indexFormat = indexFormat;
    _indexCount = indexCount;
    _dynamic = dynamic;

    // create vertex buffer
    _indexBuffer = new BGFXIndexBuffer(indexFormat, indexCount, dynamic);
}

MeshPart* MeshPart::create(Mesh* mesh, unsigned int meshIndex, Mesh::PrimitiveType primitiveType,
    Mesh::IndexFormat indexFormat, unsigned int indexCount, bool dynamic)
{
    GP_ASSERT(indexCount > 0);

    MeshPart* part = new MeshPart();
    part->_mesh = mesh;
    part->_meshIndex = meshIndex;
    part->_primitiveType = primitiveType;
    part->set(indexFormat, indexCount, dynamic);

    return part;
}

unsigned int MeshPart::getMeshIndex() const
{
    return _meshIndex;
}

Mesh::PrimitiveType MeshPart::getPrimitiveType() const
{
    return _primitiveType;
}

unsigned int MeshPart::getIndexCount() const
{
    return _indexCount;
}

Mesh::IndexFormat MeshPart::getIndexFormat() const
{
    return _indexFormat;
}

const IndexBuffer* MeshPart::getIndexBuffer() const
{
    return _indexBuffer;
}

void* MeshPart::mapIndexBuffer()
{
    return _indexBuffer->lock(0, _indexCount);
}

bool MeshPart::unmapIndexBuffer()
{
    _indexBuffer->unLock();
    return true;
}

void MeshPart::setIndexData(const void* indexData, unsigned int indexStart, unsigned int indexCount)
{
    _indexBuffer->set(indexData, indexCount, indexStart);
}

bool MeshPart::setDrawRange(uint32_t indexStart, uint32_t indexCount)
{
    return _indexBuffer->setRange(indexStart, indexCount);
}

bool MeshPart::isDynamic() const
{
    return _dynamic;
}

void MeshPart::draw()
{

}

}
