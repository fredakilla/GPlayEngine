#pragma once

#include <deque>
#include <map>
#include <array>
#include <list>
#include <atomic>
#include <mutex>

#include "../events/EventManagerBase.h"

const uint32_t NUM_QUEUES = 2u;
using EventManagerRef = std::shared_ptr<class EventManager>;


class EventManager : public EventManagerBase
{
    using EventListenerList = std::list<EventListenerDelegate>;
    using EventListenerMap	= std::map<EventID, EventListenerList>;
    using EventQueue		= std::deque<EventDataRef>;

public:

    static EventManagerRef create( const std::string &name, bool setAsGlobal );

    virtual ~EventManager();

    virtual bool addListener( const EventListenerDelegate &eventDelegate, const EventID &type ) override;
    virtual bool removeListener( const EventListenerDelegate &eventDelegate, const EventID &type ) override;

    virtual bool triggerEvent( const EventDataRef &event ) override;
    virtual bool queueEvent( const EventDataRef &event ) override;
    virtual bool abortEvent( const EventID &type, bool allOfType = false ) override;

    virtual bool addThreadedListener( const EventListenerDelegate &eventDelegate, const EventID &type ) override;
    virtual bool removeThreadedListener( const EventListenerDelegate &eventDelegate, const EventID &type ) override;
    virtual void removeAllThreadedListeners() override;
    virtual bool triggerThreadedEvent( const EventDataRef &event ) override;

    virtual bool update( uint64_t maxMillis = kINFINITE ) override;

private:
    explicit EventManager( const std::string &name, bool setAsGlobal );

    std::mutex mThreadedEventListenerMutex;
    EventListenerMap mThreadedEventListeners;

    EventListenerMap mEventListeners;
    std::array<EventQueue, NUM_QUEUES> mQueues;
    uint32_t mActiveQueue;

};


//! helper macro to create listener
#define GP_EVENT_LISTENER(listener, method) (fastdelegate::MakeDelegate(listener, &method))

//! helper macro to declare event
//! ex:
//! GP_EVENT_BEGIN(MouseEvent)
//!     Vector2 mousePosition;  // mouse pos paremeter
//!     int button;             // mouse button pressed parameter
//! GP_EVENT_END()
//!
#define GP_EVENT_BEGIN(NAME) \
class NAME : public EventData \
{ \
public: \
    GP_DECLARE_EVENT(NAME) \
    static std::shared_ptr<class NAME> create() { return  std::shared_ptr<class NAME>(new NAME); } \
private: \
    explicit NAME() : EventData() {} \
public: \

#define GP_EVENT_END() };



