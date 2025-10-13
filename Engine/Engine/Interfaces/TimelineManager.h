#pragma once
#include "Timeline.h"
#include "../Core/GameObject.h"
#include "../Singleton/TimeSingleton.h"

#include <memory>

namespace diji
{
    class TimeSingleton;
    class GameObject;

    using TimelineId = uint64_t;
    struct TimelineHandle
    {
        TimelineId id = 0;
        [[nodiscard]] bool valid() const { return id != 0; }
    };

    // todo: add automatic timeline cleanup after timeline has finished playing and is not looping
    class TimelineManager final
    {
    public:
        TimelineManager() = default;
        ~TimelineManager() noexcept = default;
        
        TimelineManager(const TimelineManager& other) = delete;
        TimelineManager(TimelineManager&& other) = delete;
        TimelineManager& operator=(const TimelineManager& other) = delete;
        TimelineManager& operator=(TimelineManager&& other) = delete;

        Timeline* CreateTimeline(const GameObject* owner);
        void ClearGameObjectTimelines(const GameObject* gameObject);
        void UpdateAll();
        void ClearAll();

    private:
        // todo: can timelines be shared between game objects? instead of creating new timelines for object with same timeline?
        const TimeSingleton& m_TimeSingletonInstancePtr = TimeSingleton::GetInstance();
        TimelineId m_nextId = 1;

        std::unordered_map<TimelineId, std::unique_ptr<Timeline>> m_Timelines;
        std::unordered_map<const GameObject*, std::vector<TimelineId>> m_OwnerToTimelines;
    };
}
