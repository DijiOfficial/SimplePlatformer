#pragma once
#include <unordered_map>
#include <functional>

namespace diji
{
    template<typename... Args>
    class Event final
    {
    public:
        using Callback = std::function<void(Args...)>;
        // todo: Constructor and destructor should be private. Look into pImpl.)
        Event() = default;
        ~Event() = default;
        
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;
        Event(Event&&) noexcept = delete;
        Event& operator=(Event&&) noexcept = delete;
        
        void AddListener(Callback cb)
        {
            m_Listeners.push_back(std::move(cb));
        }

        template<typename T>
        void AddListener(T* observer, void (T::*func)(Args...))
        {
            m_Listeners.push_back([observer, func](Args... args)
            {
                (observer->*func)(args...);
            });
        }

        template<typename T>
        void AddListener(const T* observer, void (T::*func)(Args...) const)
        {
            m_Listeners.push_back([observer, func](Args... args)
            {
                (observer->*func)(args...);
            });
        }

        // Remove a listener (optional, if you want removal support)
        // void RemoveListener(const Callback& callback) { ... }

        void Broadcast(Args... args)
        {
            for (auto& callback : m_Listeners)
            {
                callback(args...);
            }
        }

        void ClearListeners()
        {
            m_Listeners = std::vector<Callback>();
        }

    private:
        
        std::vector<Callback> m_Listeners;
    };
}


