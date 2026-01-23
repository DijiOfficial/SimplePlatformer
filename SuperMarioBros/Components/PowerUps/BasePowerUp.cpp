#include "BasePowerUp.h"

#include "../../Interfaces/IPowerUp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/SceneManager.h"
#include "../Player/PlayerCharacter.h"
#include "../../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Interfaces/IInterface.h"
#include "Engine/Singleton/Helpers.h"

superMarioBros::BasePowerUp::BasePowerUp(diji::GameObject* ownerPtr, const IPowerUp::PowerUpType powerUpType, const bool canMove, std::string pointString)
    : Component(ownerPtr)
    , m_PointString{std::move(pointString )}
    , m_PowerUpType{ powerUpType }
    , m_CanMove{ canMove }
{
}

void superMarioBros::BasePowerUp::Init()
{
    m_TransformCompPtr = GetOwner()->GetRootComponent();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    
    if (const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar"))
    {
        player->GetComponent<PlayerCharacter>()->OnHitByEnemyEvent.AddListener([this]()
        {
            m_Paused = true;
        });

        m_ColliderCompPtr->OverlapCollider(player->GetComponent<diji::Collider>());
    }

    PlayStartAnimation();
}

void superMarioBros::BasePowerUp::Start()
{
    for (const auto collider : GameManager::GetInstance().GetEnemyColliders())
        m_ColliderCompPtr->OverlapCollider(collider);
}

void superMarioBros::BasePowerUp::Update()
{
    if (m_TransformCompPtr->GetWorldPosition().y > 600.f)
        Destroy();
}

void superMarioBros::BasePowerUp::FixedUpdate()
{
    if (m_Paused || !m_CanMove) return;

    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ m_Speed, m_ColliderCompPtr->GetVelocity().y });
}

void superMarioBros::BasePowerUp::OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (other->GetTag() != "player")
        return;
    
    Destroy();

    const auto& pos = m_TransformCompPtr->GetWorldPosition();
    constexpr float yOffset = 50.f;
    const auto& scorePos = sf::Vector2f{ pos.x, pos.y - yOffset };
    GameManager::SpawnPointsText(scorePos, m_PointString);

    OnPickup(other);
}

void superMarioBros::BasePowerUp::OnHitEvent(const diji::Collider* other, const diji::CollisionInfo& hitInfo)
{
    // todo: use directions
    if (diji::Helpers::isZero(hitInfo.normal.x))
        return;

    if (std::abs(other->GetPosition().y - m_ColliderCompPtr->GetPosition().y) > 2.5f)
        return;

    m_Speed = -m_Speed;
}

void superMarioBros::BasePowerUp::PlayStartAnimation()
{
    m_TimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = m_TimelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.8f, .value= -50.f } };

    diji::Transform* transformPtr = GetOwner()->GetRootComponent();
    sf::Vector2f originalPos = transformPtr->GetWorldPosition();

    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetWorldPosition(sf::Vector2f{ originalPos.x, originalPos.y + y });
    };

    auto& [eventName, eventKeysVec] = m_TimelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.8f, .callback= [&]()
            {
                OnAnimationComplete();
            }
        }
    };
}

void superMarioBros::BasePowerUp::OnPickup(const diji::Collider* other) const
{
    const auto powerUpInterface = diji::InterfaceRegistry::GetInterface<IPowerUp>(other->GetParent());
    powerUpInterface->OnPowerUpCollected(m_PowerUpType);
}
