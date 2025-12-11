#include "BaseEnemy.h"

#include "../../Helpers/MarioHelpers.h"
#include "../Player/PlayerCharacter.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "../../Singletons/GameManager.h"
#include "../Player/BroadcastPlayerPosition.h"

void superMarioBros::BaseEnemy::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_SpriteRenderCompPtr = GetOwner()->GetComponent<diji::SpriteRenderComponent>();

    const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar");
    player->GetComponent<BroadcastPlayerPosition>()->OnPositionMileStoneReachedEvent.AddListener(this, &BaseEnemy::CheckActivation);
}

void superMarioBros::BaseEnemy::Start()
{
    const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar");
    player->GetComponent<PlayerCharacter>()->OnHitByEnemyEvent.AddListener(this, &BaseEnemy::Pause);

    player->GetComponent<PlayerCharacter>()->OnEnemyStompedEvent.AddListener(this, &BaseEnemy::HandleStomp);
    player->GetComponent<PlayerCharacter>()->OnPoweringUpEvent.AddListener(this, &BaseEnemy::SetPauseState);
}

void superMarioBros::BaseEnemy::Update()
{
    if (m_TransformCompPtr->GetPosition().y > 600.f)
        Destroy();
}

void superMarioBros::BaseEnemy::FixedUpdate()
{
    if (m_Paused) return;
    
    m_TransformCompPtr->AddOffset(m_Speed * diji::TimeSingleton::GetInstance().GetFixedUpdateDeltaTime(), 0.f);
}

void superMarioBros::BaseEnemy::CheckActivation(const int milestone) const
{
    if (m_ActivationMilestone != milestone) return;

    SetActive(true);
}

void superMarioBros::BaseEnemy::SpawnPointsText(const std::string& score) const
{
    const auto& pos = m_TransformCompPtr->GetPosition();
    const auto& yOffset = m_ColliderCompPtr->GetShape()->GetAABB().size.y * 3.f;
    const auto& scorePos = sf::Vector2f{ pos.x, pos.y - yOffset };
    GameManager::SpawnPointsText(scorePos, score);
}
