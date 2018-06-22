#include "../renderer/MemoryBuffer.h"

namespace gplay {

//---------------------------------------------------------
// IBuffer
//---------------------------------------------------------

IBuffer::IBuffer()
{
    _size = 0;
}

IBuffer::~IBuffer()
{
    destroy();
}

void IBuffer::resize(uint32_t newSize)
{
    if (_size < newSize)
        create(newSize);
    _size = newSize;
}

uint32_t IBuffer::getSize()
{
    return _size;
}

void IBuffer::create(uint32_t newSize)
{
    destroy();
    _size = newSize;
}

void IBuffer::destroy()
{
    _size = 0;
}

void* IBuffer::map(uint32_t stride)
{
    return nullptr;
}

void IBuffer::unmap()
{

}

//---------------------------------------------------------
// MemoryBuffer
//---------------------------------------------------------

MemoryBuffer::MemoryBuffer() :
    IBuffer()
  , buffer(nullptr)
{

}

MemoryBuffer::~MemoryBuffer()
{
    destroy();
}

void MemoryBuffer::create(uint32_t newSize)
{
    IBuffer::create(newSize);
    buffer = new unsigned char[newSize];
}

void MemoryBuffer::destroy()
{
    IBuffer::destroy();

    if (buffer)
    {
        delete[] buffer;
        buffer = nullptr;
    }
}

void* MemoryBuffer::map(uint32_t stride)
{
    GP_ASSERT(buffer);
    GP_ASSERT(stride < _size);
    return &buffer[stride];
}


void MemoryBuffer::unmap()
{

}

}
