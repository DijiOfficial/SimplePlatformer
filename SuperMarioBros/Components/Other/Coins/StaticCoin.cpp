#include "StaticCoin.h"

#include "../../../Helpers/MarioHelpers.h"
#include "../../../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"

void superMarioBros::StaticCoin::OnTriggerEnter(const diji::Collider* collider, const diji::CollisionInfo&)
{
    if (collider->GetTag() != "player" || m_IsCollected) return;

    m_IsCollected = true;
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_coin.wav", false);
    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(200);
    GameManager::GetInstance().OnCoinCollectedEvent.Broadcast();

    Destroy();
}

void superMarioBros::StaticCoin::HandleBumpedBehavior(const bool, const bool)
{
    if (m_IsCollected) return;
    m_IsCollected = true;
    
    MarioHelpers::SpawnCoinAboveBlock(GetOwner()->GetRootComponent()->GetWorldPosition() + sf::Vector2f{ 0.f, 50.f });
    Destroy();
}
