#include "BroadcastPlayerPosition.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"

void thomasWasLate::BroadcastPlayerPosition::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
}

void thomasWasLate::BroadcastPlayerPosition::LateUpdate()
{
    const int currentXMilestone = m_IsFirstUpdate ? 20 : static_cast<int>(std::floor(m_TransformCompPtr->GetPosition().x / static_cast<float>(BLOCK_SIZE)));
    m_IsFirstUpdate = false;
    
    if (currentXMilestone >= m_LastBroadcastedMileStone)
    {
        for (int milestone = m_LastBroadcastedMileStone + 1; milestone <= currentXMilestone; ++milestone)
        {
            OnPositionMileStoneReachedEvent.Broadcast(milestone);
        }
        
        m_LastBroadcastedMileStone = currentXMilestone;
    }
}
