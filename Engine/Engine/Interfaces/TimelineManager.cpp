#include "TimelineManager.h"
#include "../Core/GameObject.h"

#include <ranges>

diji::Timeline* diji::TimelineManager::CreateTimeline(const GameObject* owner)
{
    TimelineId id = m_nextId++;
    auto timeline = std::make_unique<Timeline>();

    m_Timelines.emplace(id, std::move(timeline));
    if (owner)
        m_OwnerToTimelines[owner].push_back(id);

    return m_Timelines[id].get();
}

void diji::TimelineManager::ClearGameObjectTimelines(const GameObject* gameObject)
{
    const auto it = m_OwnerToTimelines.find(gameObject);
    if (it == m_OwnerToTimelines.end()) return;

    for (TimelineId id : it->second)
    {
        m_Timelines.erase(id);
    }
    m_OwnerToTimelines.erase(it);
}

void diji::TimelineManager::UpdateAll()
{
    const float deltaTime = m_TimeSingletonInstancePtr.GetDeltaTime();
    for (const auto& timeline : m_Timelines | std::views::values)
    {
        timeline->Update(deltaTime);
    }
}

void diji::TimelineManager::ClearAll()
{
    m_Timelines.clear();
    m_Timelines = std::unordered_map<TimelineId, std::unique_ptr<Timeline>>();
    m_OwnerToTimelines = std::unordered_map<const GameObject*, std::vector<TimelineId>>();
}
