#pragma once

#include "../renderer/MemoryBuffer.h"

namespace gplay
{

class GeometryBuffer
{
public:
    enum LockState
    {
        LOCK_NONE = 0,
        LOCK_ACTIVE,
    };

    GeometryBuffer();
    virtual ~GeometryBuffer();
    virtual void set(const void* data, uint32_t count, uint32_t start);
    virtual void bind() const;
    virtual void* lock(uint32_t start, uint32_t count);
    virtual void unLock();
    const uint32_t getElementCount() const { return _elementCount; }
    bool isDynamic() { return _dynamic; }
    void initialize(uint32_t elementSize, uint32_t elementCount, bool dynamic);
    bool setRange(uint32_t start, uint32_t count);

protected:
    uint32_t _elementSize;      // size of 1 element
    uint32_t _elementCount;     // number of element
    MemoryBuffer _memoryBuffer; // memory buffer
    bool _dynamic;              // is dynamic
    LockState _lockState;
    uint32_t _lockStart;
    uint32_t _lockCount;
    void* _lockData;

    uint32_t _drawStart;        // draw range start
    uint32_t _drawCount;        // draw range count
};

}
