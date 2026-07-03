#include "TimerManager.h"
#include "TimeSingleton.h"
#include "../Components/Component.h"

void diji::TimerManager::Init()
{
    m_TimeSingleton = &TimeSingleton::GetInstance();
}

void diji::TimerManager::Update()
{
    for (auto it = m_Timers.begin(); it != m_Timers.end();)
    {
        Timer* timer = it->second.get();

        if (timer->TryUpdate(m_TimeSingleton->GetDeltaTime()))
            it = m_Timers.erase(it);
        else
            ++it;
    }
}

// todo: In theory this can cause an issue if the NextTickCallback is made during the frame the object is destroyed
void diji::TimerManager::UpdateNextTickCallbacks()
{
    if (m_NextTickCallbacksVec.empty() && m_PendingNextTickCallbacksVec.empty())
        return;
    
    for (auto& callback : m_NextTickCallbacksVec)
    {
        callback();
    }
    m_NextTickCallbacksVec.clear();

    m_NextTickCallbacksVec.swap(m_PendingNextTickCallbacksVec);
}

diji::TimerManager::TimerHandle diji::TimerManager::SetTimer(const Component* owner, std::function<void()> callback, const float interval, const bool isLooping, const float initialDelay)
{
    const size_t id = ++m_NextId;
    const float timeRemaining = initialDelay + interval >= 0.f ? initialDelay + interval : 0.f;
    m_Timers.emplace(id,std::make_unique<Timer>(id, owner, std::move(callback), timeRemaining, interval, isLooping));

    return id;
}

void diji::TimerManager::ClearTimer(const TimerHandle& handle)
{
    m_Timers.erase(handle);
}

void diji::TimerManager::ClearAllTimers()
{
    m_Timers = std::unordered_map<TimerHandle, std::unique_ptr<Timer>>();
    m_NextTickCallbacksVec = std::vector<std::function<void()>>();
    m_PendingNextTickCallbacksVec = std::vector<std::function<void()>>();
}

void diji::TimerManager::DelayUntilNextTick(std::function<void()> callback)
{
    m_PendingNextTickCallbacksVec.emplace_back(std::move(callback));
}
