#include "EventRegistry.h"
#include "EventSystem.h"

void diji::EventRegistry::RemoveAllListenersForObject(void* object) const
{
    for (void* eventPtr : m_AllEvents)
    {
        auto* cleaner = static_cast<IEventCleaner*>(eventPtr);
        cleaner->RemoveListener(object);
    }
}

void diji::EventRegistry::ClearAllEvents()
{
    for (void* eventPtr : m_AllEvents)
    {
        auto* cleaner = static_cast<IEventCleaner*>(eventPtr);
        cleaner->ClearAllListeners();
    }
    
    m_AllEvents = std::unordered_set<void*>();
}
