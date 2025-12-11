#include "CheckPoint.h"

#include "../../../Singletons/GameManager.h"
#include "../../Player/BroadcastPlayerPosition.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"

void superMarioBros::CheckPoint::Init()
{
    const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar");
    player->GetComponent<BroadcastPlayerPosition>()->OnPositionMileStoneReachedEvent.AddListener(this, &CheckPoint::CheckActivation);
}

void superMarioBros::CheckPoint::CheckActivation(const int milestone) const
{
    if (m_ActivationMilestone != milestone) return;

    GameManager::GetInstance().TriggerCheckPoint(GetOwner()->GetComponent<diji::Transform>()->GetPosition());
    Destroy();
}

