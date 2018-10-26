#pragma once

#include <string>
#include "../events/BaseEventData.h"
#include "../events/FastDelegate.h"

using EventID				= uint64_t;
using EventListenerDelegate = fastdelegate::FastDelegate1<EventDataRef, void>;

class EventManagerBase
{
public:

    enum eConstants { kINFINITE = 0xffffffff };
    explicit EventManagerBase( const std::string &name, bool setAsGlobal );
    virtual ~EventManagerBase();

    //! Registers a delegate function that will get called when the event type is
    //! triggered. Returns true if successful, false if not.
    virtual bool addListener( const EventListenerDelegate &eventDelegate, const EventID &type ) = 0;

    //! Removes a delegate / event type pairing from the internal tables.
    //! Returns false if the pairing was not found.
    virtual bool removeListener( const EventListenerDelegate &eventDelegate, const EventID &type ) = 0;

    //! Fires off event NOW. This bypasses the queue entirely and immediately
    //! calls all delegate functions registered for the event.
    virtual bool triggerEvent( const EventDataRef &event ) = 0;

    //! Fires off event. This uses the queue and will call the delegate
    //! function on the next call to tickUpdate. assuming there's enough time.
    virtual bool queueEvent( const EventDataRef &event ) = 0;

    //! Finds the next-available instance of the named event type and remove it
    //! from the processing queue. This may be done up to the point that it is
    //! actively being processed ... e.g.: is safe to happen during event
    //! processing itself.
    //!
    //! If allOfType is true, then all events of that type are cleared from the
    //! input queue.
    //! returns true if the event was found and removed, false otherwise
    virtual bool abortEvent( const EventID& type, bool allOfType = false ) = 0;

    //! Allows for processing of any queued messages, optionally specify a
    //! processing time limit so that the event processing does not take too
    //! long. Note the danger of using this artificial limiter is that all
    //! messages may not in fact get processed.
    //!
    //! returns true if all messages ready for processing were completed, false
    //! otherwise (e.g. timeout).
    virtual bool update( uint64_t maxMillis = kINFINITE ) = 0;

    //! Getter for the main global event manager. This is the event manager that
    //! is used by the majority of the engine, though you are free to define your
    //! own as long as you instantiate it with setAsGlobal set to false.
    //! It is not valid to have more than one global event manager.
    static EventManagerBase* getInstance();

    //! Registers a delegate function that will get called when the event type is
    //! triggered. NOTE: This listener can be called from any thread. Appropriate
    //! locks in the listener should be considered. Returns true if successful,
    //! false if not. This function is Thread Safe
    virtual bool addThreadedListener( const EventListenerDelegate &eventDelegate, const EventID &type ) = 0;

    //! Removes a delegate / event type pairing from the internal tables. This
    //! function removes in a Thread Safe manner. Returns false if the pairing
    //! was not found.
    virtual bool removeThreadedListener( const EventListenerDelegate &eventDelegate, const EventID &type ) = 0;

    //! Fires off event NOW. NOTE: This function could be called from any thread.
    //! This bypasses the queue entirely and immediately calls all delegate functions
    //! registered to listen for this event.
    virtual bool triggerThreadedEvent( const EventDataRef &event ) = 0;

    //! Removes all delegates.
    virtual void removeAllThreadedListeners() = 0;
};
