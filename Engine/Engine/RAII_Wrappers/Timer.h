#pragma once
#include <functional>

namespace diji
{
    class TimerManager;
    class Component;

    class Timer
    {
    public:
        using Callback = std::function<void()>;

        Timer() = delete;
        Timer(size_t id, const Component* owner, Callback callback, float timeRemaining, float interval, bool looping);
        ~Timer();

        Timer(const Timer&) = delete;
        Timer& operator=(const Timer&) = delete;
        Timer(Timer&& other) noexcept;
        Timer& operator=(Timer&& other) noexcept;

        size_t GetId() const { return m_ID; }
        bool TryUpdate(float dt);

    private:
        size_t m_ID = 0;
        const Component* m_Owner = nullptr;

        Callback m_Callback;
        float m_TimeRemaining = 0.f;
        float m_Interval = 0.f;
        bool m_Looping = false;
    };
}
