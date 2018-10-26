#include "../events/EventManagerBase.h"
#include "../core/Base.h"

static EventManagerBase* kEventManager = nullptr;

EventManagerBase* EventManagerBase::getInstance()
{
    return kEventManager;
}

EventManagerBase::EventManagerBase( const std::string &name, bool setAsGlobal )
{
    if(setAsGlobal)
    {
        if(kEventManager)
        {
            GP_WARN("Attempting to make two Event Managers. Deleting the old one and replacing with the new one");
            delete kEventManager;
        }
        kEventManager = this;
    }
}

EventManagerBase::~EventManagerBase()
{
    /*if(kEventManager)
    {
        auto tempManagerForDelete = kEventManager;
        kEventManager = nullptr;
        delete tempManagerForDelete;
    }*/
}
