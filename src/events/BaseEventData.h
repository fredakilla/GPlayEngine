#pragma once

#include <memory>

using EventDataRef = std::shared_ptr<class EventData>;
using EventID = uint64_t;

class EventData
{
public:
    explicit EventData() {}
    virtual ~EventData() {}
    typedef EventID id_t;
    virtual id_t getEventID() const = 0;
    virtual const char* getName() const = 0;    // used only for debug
};


//! macro for automatic overriding needed methods in derived classes
#define GP_DECLARE_EVENT(type) \
    static EventData::id_t ID() { return reinterpret_cast<EventData::id_t>(&ID); } \
    EventData::id_t getEventID() const override { return ID(); } \
    const char* getName() const override { return #type; } \


