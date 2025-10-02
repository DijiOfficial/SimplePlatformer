#include "TimerManager.h"
#include "TimeSingleton.h"

void diji::TimerManager::Init()
{
    m_TimeSingleton = &diji::TimeSingleton::GetInstance();
}

void diji::TimerManager::Update()
{
    const float deltaTime = m_TimeSingleton->GetDeltaTime();
    for (auto it = m_Timers.begin(); it != m_Timers.end();)
    {
        it->timeRemaining -= deltaTime;
        if (it->timeRemaining <= 0.0f)
        {
            if (it->callback)
                it->callback();

            if (it->looping)
            {
                it->timeRemaining += it->interval;
                ++it;
            }
            else
            {
                it = m_Timers.erase(it);
            }
        }
        else
        {
            ++it;
        }
    }
}

diji::TimerManager::TimerHandle diji::TimerManager::SetTimer(std::function<void()> callback, const float interval, const bool isLooping, const float initialDelay)
{
    Timer timer;
    timer.id = m_NextId++;
    timer.callback = std::move(callback);
    timer.timeRemaining = (initialDelay > 0.0f) ? initialDelay + interval : interval;
    timer.interval = interval;
    timer.looping = isLooping;

    m_Timers.push_back(timer);

    return TimerHandle{ timer.id };
}

void diji::TimerManager::ClearTimer(const TimerHandle& handle) // Todo: consider using a map for efficiency (bigger number of timers) 
{
    const auto& it = std::ranges::find_if(m_Timers, [&](const Timer& timer)
    {
        return timer.id == handle.id;
    });

    if (it != m_Timers.end())
        m_Timers.erase(it);
}

void diji::TimerManager::ClearAllTimers()
{
    m_Timers = std::vector<Timer>();
}
