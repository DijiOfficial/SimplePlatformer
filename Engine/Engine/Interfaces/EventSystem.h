#pragma once
#include "EventRegistry.h"
#include <functional>
#include <vector>
#include <algorithm>

namespace diji
{
    template<typename... Args>
    class Event final : public IEventCleaner
    {
    public:
        using Callback = std::function<void(Args...)>;
        using ListenerId = size_t;
        
        Event() noexcept { EventRegistry::GetInstance().RegisterEvent(this); }
        ~Event() noexcept override { EventRegistry::GetInstance().UnregisterEvent(this); }
        
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;
        Event(Event&&) noexcept = delete;
        Event& operator=(Event&&) noexcept = delete;
        
        template<typename T>
        ListenerId AddListener(T* observer, void (T::*func)(Args...))
        {
            const ListenerId id = m_NextId++;
            auto callback = [observer, func](Args... args)
            {
                (observer->*func)(args...);
            };
            
            m_Listeners.emplace_back(id, observer, std::move(callback));
            return id;
        }

        template<typename T>
        ListenerId AddListener(const T* observer, void (T::*func)(Args...) const)
        {
            const ListenerId id = m_NextId++;
            auto callback = [observer, func](Args... args)
            {
                (observer->*func)(args...);
            };
            
            m_Listeners.emplace_back(id, const_cast<T*>(observer), std::move(callback));
            return id;
        }

        ListenerId AddListener(Callback cb)
        {
            const ListenerId id = m_NextId++;
            m_Listeners.emplace_back(id, nullptr, std::move(cb)); // nullptr = lambda/function
            return id;
        }

        bool RemoveListener(ListenerId id)
        {
            auto it = std::find_if(m_Listeners.begin(), m_Listeners.end(),
                [id](const auto& listener) { return std::get<0>(listener) == id; });
            
            if (it != m_Listeners.end())
            {
                m_Listeners.erase(it);
                return true;
            }
            return false;
        }

        // IEventCleaner interface - called by EventRegistry
        void RemoveListener(void* object) override
        {
            auto it = std::remove_if(m_Listeners.begin(), m_Listeners.end(),
                [object](const auto& listener) { return std::get<1>(listener) == object; });
            m_Listeners.erase(it, m_Listeners.end());
        }

        void ClearAllListeners() override
        {
            m_Listeners.clear();
        }

        void Broadcast(Args... args)
        {
            for (auto& [id, observer, callback] : m_Listeners)
            {
                callback(args...);
            }
        }

        [[nodiscard]] size_t GetListenerCount() const { return m_Listeners.size(); }

    private:
        std::vector<std::tuple<ListenerId, void*, Callback>> m_Listeners;
        ListenerId m_NextId = 0;
    };
}


// #pragma once
// #include <unordered_map>
// #include <functional>
//
// namespace diji
// {
//     template<typename... Args>
//     class Event final
//     {
//     public:
//         using Callback = std::function<void(Args...)>;
//         // todo: Constructor and destructor should be private. Look into pImpl.)
//         Event() = default;
//         ~Event() = default;
//         
//         Event(const Event&) = delete;
//         Event& operator=(const Event&) = delete;
//         Event(Event&&) noexcept = delete;
//         Event& operator=(Event&&) noexcept = delete;
//         
//         void AddListener(Callback cb)
//         {
//             m_Listeners.push_back(std::move(cb));
//         }
//
//         template<typename T>
//         void AddListener(T* observer, void (T::*func)(Args...))
//         {
//             m_Listeners.push_back([observer, func](Args... args)
//             {
//                 (observer->*func)(args...);
//             });
//         }
//
//         template<typename T>
//         void AddListener(const T* observer, void (T::*func)(Args...) const)
//         {
//             m_Listeners.push_back([observer, func](Args... args)
//             {
//                 (observer->*func)(args...);
//             });
//         }
//
//         // Remove a listener (optional, if you want removal support)
//         // void RemoveListener(const Callback& callback) { ... }
//
//         void Broadcast(Args... args)
//         {
//             for (auto& callback : m_Listeners)
//             {
//                 callback(args...);
//             }
//         }
//
//         void ClearListeners()
//         {
//             m_Listeners = std::vector<Callback>();
//         }
//
//     private:
//         std::vector<Callback> m_Listeners;
//     };
// }
//
//
