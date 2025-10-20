#include "BroadcastPlayerPosition.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"

void thomasWasLate::BroadcastPlayerPosition::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
}

void thomasWasLate::BroadcastPlayerPosition::LateUpdate()
{
    const int currentXMilestone = static_cast<int>(std::floor(m_TransformCompPtr->GetPosition().x / BLOCK_SIZE));

    if (currentXMilestone >= m_LastBroadcastedMileStone)
    {
        for (int milestone = m_LastBroadcastedMileStone + 1; milestone <= currentXMilestone; ++milestone)
        {
            OnPositionMileStoneReachedEvent.Broadcast(milestone);
        }
        
        m_LastBroadcastedMileStone = currentXMilestone;
    }
}
