#include "Timeline.h"
#include "../Singleton/Helpers.h"

#include <stdexcept>

float diji::FloatTrack::Evaluate(const float time) const
{
    if (keys.empty()) return 0.f;

    // before first key
    if (time <= keys.front().time)
        return keys.front().value;
    
    // after last key
    if (time >= keys.back().time)
        return keys.back().value;

    // todo: potential optimization to avoid O(n) search
    for (size_t i = 0; i + 1 < keys.size(); ++i)
    {
        const Keyframe& a = keys[i];
        const Keyframe& b = keys[i + 1];
        if (time >= a.time && time <= b.time)
        {
            const float dt = b.time - a.time;
            if (dt <= 0.f)
                return a.value;

            const float alpha = (time - a.time) / dt;
            return Helpers::lerp(a.value, b.value, alpha);
        }
    }

    // fallback
    throw std::runtime_error("FloatTrack::Evaluate - should not reach here");
}

void diji::Timeline::PlayFromStart()
{
    m_Time = m_Reverse ? m_Length : 0.f;
        
    for (auto& [name, keys] : m_EventTracks)
        for (auto& key : keys)
            key.fired = false;
        
    m_Finished = false;
    m_Playing = true;
}

void diji::Timeline::Stop()
{
    m_Playing = false;
    m_Time = m_Reverse ? m_Length : 0.f;
    
    // reset event flags
    for (auto& [name, keys] : m_EventTracks)
        for (auto& key : keys)
            key.fired = false;
    
    m_Finished = false;
}

void diji::Timeline::JumpToTime(const float time, const bool fireEvents)
{
    const float newTime =  std::max(0.f, std::min(time, m_Length));
    const float prevTime = m_Time;
    m_Time = newTime;

    if (!fireEvents)
    {
        // Reset fired state when seek without firing, this would exclusively be used for scrubbing if I ever add a UI for it
        for (auto& [name, keys] : m_EventTracks)
            for (auto& key : keys)
                key.fired = false;
        
        return;
    }

    if (Helpers::AreFloatEqual(prevTime, newTime)) return;

    if (newTime > prevTime)
    {
        // while moving forward, fire all events in [prevTime, newTime]
        for (auto& [name, keys] : m_EventTracks)
            for (auto& [keyTime, callback, isFired] : keys)
                if (!isFired && keyTime >= prevTime && keyTime <= newTime)
                {
                    if (callback) callback();
                    isFired = true;
                }
    }
    else
    {
        // Same thing but for reversed timelines
        // iterate reverse to keep consistent order for reverse firing semantics
        for (auto& [name, keys] : m_EventTracks)
            for (auto it = keys.rbegin(); it != keys.rend(); ++it)
                if (!it->fired && it->time <= prevTime && it->time >= newTime)
                {
                    if (it->callback) it->callback();
                    it->fired = true;
                }
    }
}

diji::FloatTrack& diji::Timeline::AddFloatTrack(const std::string& name)
{
    m_FloatTracks.emplace_back();
    m_FloatTracks.back().name = name;
    
    RecalculateLength();
    
    return m_FloatTracks.back();
}

diji::EventTrack& diji::Timeline::AddEventTrack(const std::string& name)
{
    m_EventTracks.emplace_back();
    m_EventTracks.back().name = name;

    RecalculateLength();
    
    return m_EventTracks.back();
}

void diji::Timeline::Update(const float dt)
{
    // todo: not good, figure out a better way to handle this
    for (const auto& floatTrack : m_FloatTracks)
    {
        if (floatTrack.needsUpdate)
            RecalculateLength();
    }
    
    if (!m_Playing || m_Length <= 0.f || m_Finished) return;

    // Apply play rate and direction
    const float deltaTime = dt * m_PlayRate * (m_Reverse ? -1.f : 1.f);
    const float newTime = m_Time + deltaTime;

    // Event firing: handle forward and reverse crossing
    if (!m_Reverse)
    {
        // todo: template this to avoid code duplication
        for (auto& [name, keys] : m_EventTracks)
            for (auto& [keyTime, callback, fired] : keys)
                if (!fired && keyTime >= m_Time && keyTime <= newTime)
                {
                    if (callback) callback();
                    fired = true;
                }
    }
    else
    {
        for (auto& [name, keys] : m_EventTracks)
            for (auto it = keys.rbegin(); it != keys.rend(); ++it)
                if (!it->fired && it->time <= m_Time && it->time >= newTime)
                {
                    if (it->callback) it->callback();
                    it->fired = true;
                }
    }

    m_Time = newTime;
    
    if (m_Loop && m_Length > 0.f)
    {
        // Looping and time wrapping
        if (m_Time >= m_Length)
        {
            // wrap around but preserve fractional overrun
            m_Time = fmod(m_Time, m_Length);
            
            // you know the drill
            for (auto& [name, keys] : m_EventTracks)
                for (auto& key : keys)
                    key.fired = false;
        }
        else if (m_Time < 0.f)
        {
            // negative wrap
            m_Time = m_Length - fmod(std::fabs(m_Time), m_Length);
            for (auto& [name, keys] : m_EventTracks)
                for (auto& key : keys)
                    key.fired = false;
        }
    }
    else
    {
        // Non-looping: clamp and stop at ends
        if (!m_Reverse)
        {
            if (m_Time >= m_Length)
            {
                m_Time = m_Length;
                m_Playing = false;
                m_Finished = true;
            }
            else if (m_Time <= 0.f)
            {
                m_Time = 0.f;
                // keep playing?
            }
        }
        else
        {
            if (m_Time <= 0.f)
            {
                m_Time = 0.f;
                m_Playing = false;
                m_Finished = true;
            }
            else if (m_Time >= m_Length)
            {
                m_Time = m_Length;
            }
        }
    }

    for (auto& floatTrack : m_FloatTracks)
    {
        const float value = floatTrack.Evaluate(m_Time);

        if (floatTrack.onValue)
            floatTrack.onValue(value);
    }
}

void diji::Timeline::RecalculateLength()
{
    float maxT = 0.f;
    for (auto& floatTrack : m_FloatTracks)
    {
        for (const auto& [time, value] : floatTrack.keys)
        {
            maxT = std::max(time, maxT);
        }
    }
    
    for (auto& [name, keys] : m_EventTracks)
    {
        for (auto &key : keys)
        {
            maxT = std::max(key.time, maxT);
        }
    }
    
    m_Length = maxT;
    
    m_Time = std::min(m_Time, m_Length);
}
