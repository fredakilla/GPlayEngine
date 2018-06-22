#include "../renderer/GeometryBuffer.h"

namespace gplay
{

GeometryBuffer::GeometryBuffer() :
    _elementSize(0)
    , _elementCount(0)
    , _dynamic(false)
    , _lockState(LOCK_NONE)
    , _lockStart(0)
    , _lockCount(0)
    , _lockData(nullptr)
    , _drawStart(0)
    , _drawCount(0)
{
}

GeometryBuffer::~GeometryBuffer()
{
    _memoryBuffer.destroy();
}

void GeometryBuffer::initialize(uint32_t elementSize, uint32_t elementCount, bool dynamic)
{
    _elementSize = elementSize;
    _elementCount = elementCount;
    _dynamic = dynamic;

    // allocate memory buffer
    uint32_t size = _elementSize * _elementCount;
    _memoryBuffer.create(size);
}

bool GeometryBuffer::setRange(uint32_t start, uint32_t count)
{
    if(start + count > _elementCount)
    {
        GP_ERROR("Illegal buffer range : start + count > maxcount => (%d+%d > %d)", start, count, _elementCount);
        return false;
    }

    _drawStart = start;
    _drawCount = count;

    return true;
}

void GeometryBuffer::set(const void* data, uint32_t count, uint32_t start)
{
    count = count == 0 ? _elementCount : count;

    bool needResize = start + count > _elementCount;

    if(_dynamic)
    {
        // need to resize memory buffer ?
        if(needResize)
        {
            _elementCount = start + count;
            uint32_t newSize = _elementSize * _elementCount;
            if(newSize > _memoryBuffer.getSize())
                _memoryBuffer.resize(newSize);
        }
    }
    else
    {
        if(needResize)
        {
            GP_ERROR("Buffer overflow, static buffer is not allowed to resize.");
            return;
        }
    }

    setRange(start, count);

    // copy data into memory buffer
    if(data)
    {
        uint32_t memSize = _elementSize * count;
        GP_ASSERT(memSize <= _memoryBuffer.getSize());
        memcpy(_memoryBuffer.map(start), data, memSize);
    }
}

void GeometryBuffer::bind() const
{
}

void* GeometryBuffer::lock(uint32_t start, uint32_t count)
{
    _lockCount = count == 0 ? _elementCount : count;
    _lockStart = start;

    GP_ASSERT(_lockStart + _lockCount <= _elementCount);
    GP_ASSERT(_memoryBuffer.getSize() >= _elementSize * (_lockStart + _lockCount));

    if (_lockState != LOCK_NONE)
    {
        GP_ERROR("Buffer already locked.");
        return nullptr;
    }

    _lockState = LOCK_ACTIVE;
    _lockData = _memoryBuffer.map(_lockStart);

    return _lockData;
}

void GeometryBuffer::unLock()
{
    if (_lockState == LOCK_ACTIVE)
    {
        _memoryBuffer.unmap();
        _lockState = LOCK_NONE;
        _lockData = nullptr;
    }
    else
    {
        GP_ERROR("Buffer was not locked.");
    }
}

} // end namespace gplay
