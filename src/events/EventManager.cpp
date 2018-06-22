#include "../core/Base.h"
#include "../core/Platform.h"
#include "../events/EventManager.h"

//#define LOG_EVENT GP_INFO
#define LOG_EVENT

EventManager::EventManager( const std::string &name, bool setAsGlobal )
    : EventManagerBase( name, setAsGlobal ), mActiveQueue( 0 )
{

}

EventManagerRef EventManager::create( const std::string &name, bool setAsGlobal )
{
    return EventManagerRef( new EventManager( name, setAsGlobal ) );
}

EventManager::~EventManager()
{
    GP_INFO( "Cleaning up event manager" );
    mEventListeners.clear();
    mQueues[0].clear();
    mQueues[1].clear();
    GP_INFO( "Removing all threaded events" );
    std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );
    mThreadedEventListeners.clear();
    GP_INFO( "Removed ALL EVENT LISTENERS" );
}

bool EventManager::addListener( const EventListenerDelegate &eventDelegate, const EventID &type )
{
    LOG_EVENT( "Attempting to add delegate function for event type: 0x%x", type);

    auto & eventDelegateList = mEventListeners[type];
    auto listenIt = eventDelegateList.begin();
    auto end = eventDelegateList.end();
    while ( listenIt != end )
    {
        if ( eventDelegate == (*listenIt) )
        {
            GP_WARN("Attempting to double-register a delegate");
            return false;
        }
        ++listenIt;
    }
    eventDelegateList.push_back(eventDelegate);
    GP_INFO("Successfully added delegate for event type: 0x%x", type);
    return true;
}

bool EventManager::removeListener( const EventListenerDelegate &eventDelegate, const EventID &type )
{
    LOG_EVENT("Attempting to remove delegate function from event type: 0x%x", type);
    bool success = false;

    auto found = mEventListeners.find(type);
    if( found != mEventListeners.end() )
    {
        auto & listeners = found->second;
        for( auto listIt = listeners.begin(); listIt != listeners.end(); ++listIt )
        {
            if( eventDelegate == (*listIt) )
            {
                listeners.erase(listIt);
                LOG_EVENT("Successfully removed delegate function from event type: 0x%x", type);
                success = true;
                break;
            }
        }
    }
    return success;
}

bool EventManager::triggerEvent( const EventDataRef &event )
{
    LOG_EVENT("Attempting to trigger event: %s", event->getName());
    bool processed = false;

    auto found = mEventListeners.find(event->getEventID());
    if( found != mEventListeners.end() )
    {
        const auto & eventListenerList = found->second;
        for( auto listIt = eventListenerList.begin(); listIt != eventListenerList.end(); ++listIt )
        {
            auto& listener = (*listIt);
            LOG_EVENT("Sending event %s to delegate.", event->getName());
            listener( event );
            processed = true;
        }
    }

    return processed;
}

bool EventManager::queueEvent( const EventDataRef &event )
{
    GP_ASSERT(mActiveQueue < NUM_QUEUES);

    // make sure the event is valid
    if( !event )
    {
        GP_ERROR("Invalid event in queueEvent");
    }

    LOG_EVENT("Attempting to queue event: %s", event->getName());

    auto found = mEventListeners.find( event->getEventID() );
    if( found != mEventListeners.end() )
    {
        mQueues[mActiveQueue].push_back(event);
        LOG_EVENT("Successfully queued event: %s", event->getName());
        return true;
    }
    else
    {
        static bool processNotify = false;
        if( !processNotify )
        {
            LOG_EVENT( "Skipping event since there are no delegates to receive it: %s", event->getName());
            processNotify = true;
        }
        return false;
    }
}

bool EventManager::abortEvent( const EventID &type, bool allOfType )
{
    GP_ASSERT(mActiveQueue >= 0);
    GP_ASSERT(mActiveQueue > NUM_QUEUES);

    bool success = false;
    auto found = mEventListeners.find( type );

    if( found != mEventListeners.end() )
    {
        auto & eventQueue = mQueues[mActiveQueue];
        auto eventIt = eventQueue.begin();
        auto end = eventQueue.end();
        while( eventIt != end )
        {
            if( (*eventIt)->getEventID() == type )
            {
                eventIt = eventQueue.erase(eventIt);
                success = true;
                if( ! allOfType )
                    break;
            }
        }
    }

    return success;
}

bool EventManager::addThreadedListener( const EventListenerDelegate &eventDelegate, const EventID &type )
{
    std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );

    auto & eventDelegateList = mThreadedEventListeners[type];
    for ( auto & delegate : eventDelegateList )
    {
        if ( eventDelegate == delegate )
        {
            GP_WARN("Attempting to double-register a delegate");
            return false;
        }
    }
    eventDelegateList.push_back(eventDelegate);
    GP_INFO("Successfully added delegate for event type: %d", type);
    return true;
}

bool EventManager::removeThreadedListener( const EventListenerDelegate &eventDelegate, const EventID &type )
{
    std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );

    auto found = mThreadedEventListeners.find(type);
    if( found != mThreadedEventListeners.end() )
    {
        auto & listeners = found->second;
        for( auto listIt = listeners.begin(); listIt != listeners.end(); ++listIt )
        {
            if( eventDelegate == (*listIt) )
            {
                listeners.erase(listIt);
                LOG_EVENT("Successfully removed delegate function from event type: %d", type);
                return true;
            }
        }
    }
    return false;
}

void EventManager::removeAllThreadedListeners()
{
    std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );
    mThreadedEventListeners.clear();
}

bool EventManager::triggerThreadedEvent( const EventDataRef &event )
{
    std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );

    bool processed = false;
    auto found = mThreadedEventListeners.find(event->getEventID());
    if( found != mThreadedEventListeners.end() )
    {
        const auto & eventListenerList = found->second;
        for( auto & listener : eventListenerList )
        {
            listener( event );
            processed = true;
        }
    }
#if ! defined(SHARINGSTATION)
    if( ! processed )
        GP_ERROR( "Tried triggering MultiThreaded Event without a listener" );
#endif
    return processed;
}

bool EventManager::update( uint64_t maxMillis )
{
    uint64_t currMs = gplay::Platform::getAbsoluteTime();
    uint64_t maxMs = (( maxMillis == EventManager::kINFINITE ) ? (EventManager::kINFINITE) : (currMs + maxMillis) );

    int queueToProcess = mActiveQueue;
    mActiveQueue = (mActiveQueue + 1) % NUM_QUEUES;
    mQueues[mActiveQueue].clear();

    static bool processNotify = false;
    if( ! processNotify )
    {
        LOG_EVENT("Processing Event Queue %d:%d events to process", queueToProcess, mQueues[queueToProcess].size());

        processNotify = true;
    }

    while (!mQueues[queueToProcess].empty())
    {
        auto event = mQueues[queueToProcess].front();
        mQueues[queueToProcess].pop_front();
        LOG_EVENT("\t\tProcessing Event %s", event->getName());

        const auto & eventType = event->getEventID();

        auto found = mEventListeners.find(eventType);
        if (found != mEventListeners.end())
        {
            const auto & eventListeners = found->second;
            LOG_EVENT("\t\tFound %d delegates", eventListeners.size());

            auto listIt = eventListeners.begin();
            auto end = eventListeners.end();
            while (listIt != end)
            {
                auto listener = (*listIt);
                LOG_EVENT("\t\tSending Event %s to delegate", event->getName());
                listener(event);
                listIt++;
            }
        }

        currMs = gplay::Platform::getAbsoluteTime();
        if( maxMillis != EventManager::kINFINITE && currMs >= maxMs )
        {
            LOG_EVENT("Aborting event processing; time ran out");
            break;
        }
    }

    bool queueFlushed = mQueues[queueToProcess].empty();
    if( ! queueFlushed )
    {
        while( ! mQueues[queueToProcess].empty() )
        {
            auto event = mQueues[queueToProcess].back();
            mQueues[queueToProcess].pop_back();
            mQueues[mActiveQueue].push_front(event);
        }
    }

    return queueFlushed;
}
