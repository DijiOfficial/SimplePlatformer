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
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
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
    if (m_TransformCompPtr->GetPosition().y > 600.f)
        Destroy();
}

void superMarioBros::BasePowerUp::FixedUpdate()
{
    if (m_Paused || !m_CanMove) return;

    m_TransformCompPtr->AddOffset(m_Speed * diji::TimeSingleton::GetInstance().GetFixedUpdateDeltaTime(), 0.f);
}

void superMarioBros::BasePowerUp::OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (other->GetTag() != "player")
        return;
    
    Destroy();

    const auto& pos = m_TransformCompPtr->GetPosition();
    constexpr float yOffset = 50.f;
    const auto& scorePos = sf::Vector2f{ pos.x, pos.y - yOffset };
    GameManager::SpawnPointsText(scorePos, m_PointString);

    OnPickup(other);
}

void superMarioBros::BasePowerUp::OnHitEvent(const diji::Collider*, const diji::CollisionInfo& hitInfo)
{
    // todo: use directions
    if (diji::Helpers::isZero(hitInfo.normal.x))
        return;

    m_Speed = -m_Speed;
}

void superMarioBros::BasePowerUp::PlayStartAnimation()
{
    m_TimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = m_TimelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.8f, .value= -50.f } };

    diji::Transform* transformPtr = GetOwner()->GetComponent<diji::Transform>();
    sf::Vector2f originalPos = transformPtr->GetPosition();

    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetPosition(originalPos.x, originalPos.y + y);
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
