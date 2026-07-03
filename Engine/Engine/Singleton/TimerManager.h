#pragma once
#include "Singleton.h"
#include "../RAII_Wrappers/Timer.h"

#include <functional>
#include <memory>

namespace diji
{
    class TimeSingleton;
    class Component;

    // Ideally would be managed by the Scene/SceneManager
    class TimerManager final : public Singleton<TimerManager>
    {
    public:
        void Init();
        void Update();
        void UpdateNextTickCallbacks();

        // Timer handle (for canceling or checking)
        using TimerHandle = size_t;

        TimerHandle SetTimer(const Component* owner, std::function<void()> callback, float interval, bool isLooping = false, float initialDelay = 0.0f);
        void ClearTimer(const TimerHandle& handle);
        void ClearAllTimers();
        void DelayUntilNextTick(std::function<void()> callback);

    private:
        TimeSingleton* m_TimeSingleton = nullptr;
        std::unordered_map<TimerHandle, std::unique_ptr<Timer>> m_Timers;
        size_t m_NextId = 1;

        std::vector<std::function<void()>> m_NextTickCallbacksVec;
        std::vector<std::function<void()>> m_PendingNextTickCallbacksVec;
    };
}
