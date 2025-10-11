#pragma once
#include "../Singleton/Singleton.h"

#include <unordered_set>

namespace diji
{
    class GameObject;
    class Component;
    
    template<typename... Args>
    class Event;

    class EventRegistry final : public Singleton<EventRegistry>
    {
    public:
        // Register an event with the system
        template<typename... Args>
        void RegisterEvent(Event<Args...>* event) 
        {
            m_AllEvents.insert(event);
        }

        // Unregister an event (when event is destroyed)
        template<typename... Args>
        void UnregisterEvent(Event<Args...>* event) 
        {
            m_AllEvents.erase(event);
        }

        // Remove all listeners associated with a GameObject or Component
        void RemoveAllListenersForObject(void* object) const;
        void ClearAllEvents();

    private:
        // Store as void* to handle any Event<Args...>
        std::unordered_set<void*> m_AllEvents;
    };

    // Base interface for type-erased event cleanup
    class IEventCleaner 
    {
    public:
        virtual ~IEventCleaner() noexcept = default;

        IEventCleaner(const IEventCleaner&) = delete;
        IEventCleaner& operator=(const IEventCleaner&) = delete;
        IEventCleaner(IEventCleaner&&) = delete;
        IEventCleaner& operator=(IEventCleaner&&) = delete;
        
        virtual void RemoveListener(void* object) = 0;
        virtual void ClearAllListeners() = 0;
        
    protected:
        IEventCleaner() = default;
    };
}

