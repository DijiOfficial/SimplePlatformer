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
        template<typename... Args>
        void RegisterEvent(Event<Args...>* event) 
        {
            m_AllEvents.insert(event);
        }
        
        template<typename... Args>
        void UnregisterEvent(Event<Args...>* event) 
        {
            m_AllEvents.erase(event);
        }
        
        void RemoveAllListenersForObject(void* object) const;
        void ClearAllEvents();

    private:
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

