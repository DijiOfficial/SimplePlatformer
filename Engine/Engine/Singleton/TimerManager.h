#pragma once
#include "Singleton.h"

#include <functional>

namespace diji
{
    class TimeSingleton;

    // todo: Link every callback to an id linked to a GameObject and destroy all timers linked to that object when it gets destroyed!
    // Ideally would be managed by the Scene/SceneManager
    class TimerManager final : public Singleton<TimerManager>
    {
    public:
        void Init();
        void Update();
        void UpdateNextTickCallbacks();

        // Timer handle (for canceling or checking)
        struct TimerHandle
        {
            size_t id = 0;
        };

        // todo: Use template for better and cleaner code, make use create a TimerHandle and pass it instead of returning one. because I'm just voiding it
        [[nodiscard]] TimerHandle SetTimer(std::function<void()> callback, float interval, bool isLooping = false, float initialDelay = 0.0f);
        void ClearTimer(const TimerHandle& handle);
        void ClearAllTimers();
        void DelayUntilNextTick(std::function<void()> callback);

    private:
        TimeSingleton* m_TimeSingleton = nullptr;
        struct Timer
        {
            size_t id;
            std::function<void()> callback;
            float timeRemaining;
            float interval;
            bool looping;
        };

        // todo: use a deque or linked list to avoid allocations issues
        std::vector<Timer> m_Timers;
        std::vector<Timer> m_PendingTimers;
        size_t m_NextId = 1;

        std::vector<std::function<void()>> m_NextTickCallbacksVec;
        std::vector<std::function<void()>> m_PendingNextTickCallbacksVec;
    };
}
