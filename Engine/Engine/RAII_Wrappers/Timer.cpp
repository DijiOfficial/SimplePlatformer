#include "Timer.h"
#include "../Components/Component.h"
diji::Timer::Timer(const size_t id, const Component* owner, Callback callback, const float timeRemaining, const float interval, const bool looping)
    : m_ID{ id }
    , m_Owner{ owner }
    , m_Callback{ std::move(callback) }
    , m_TimeRemaining{ timeRemaining }
    , m_Interval{ interval }
    , m_Looping{ looping }
{
    m_Owner->RegisterTimerHandle(m_ID);
}

diji::Timer::~Timer()
{
    if (m_Owner)
        m_Owner->DeregisterTimerHandle(m_ID);
}

diji::Timer::Timer(Timer&& other) noexcept
    : m_ID(other.m_ID)
    , m_Owner(other.m_Owner)
    , m_Callback(std::move(other.m_Callback))
    , m_TimeRemaining(other.m_TimeRemaining)
    , m_Interval(other.m_Interval)
    , m_Looping(other.m_Looping)
{
}

diji::Timer& diji::Timer::operator=(Timer&& other) noexcept
{
    if (this == &other) return *this;

    m_ID = other.m_ID;

    return *this;
}

bool diji::Timer::TryUpdate(const float dt)
{
    m_TimeRemaining -= dt;

    if (m_TimeRemaining > 0.f)
        return false;
    
    m_Callback();

    if (m_Looping)
    {
        m_TimeRemaining += m_Interval;
        return false;
    }
    return true;
}
