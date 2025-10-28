#include "MushroomScript.h"

#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/SceneManager.h"
#include "../Player/PlayerCharacter.h"
#include "../../Singletons/GameManager.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Singleton/TimeSingleton.h"

void thomasWasLate::MushroomScript::Init()
{
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_ColliderCompPtr->SetTag("powerUp");

    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();

    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnHitByEnemyEvent.AddListener([this]()
    {
        m_Paused = true;
    });

    PlayStartAnimation();
}

void thomasWasLate::MushroomScript::Update()
{
    if (m_TransformCompPtr->GetPosition().y > 600.f)
        Destroy();
}

void thomasWasLate::MushroomScript::FixedUpdate()
{
    if (m_Paused) return;
    
    m_TransformCompPtr->AddOffset(m_Speed * diji::TimeSingleton::GetInstance().GetFixedUpdateDeltaTime(), 0.f);
}

void thomasWasLate::MushroomScript::OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo& hitInfo)
{
    if (other->GetTag() == "player")
    {
        m_Paused = false;
        OnHitEvent(other, hitInfo);
    }
}

void thomasWasLate::MushroomScript::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (m_Paused) return;
    if (collider->GetTag() == "player")
    {
        Destroy();
        
        const auto& pos = m_TransformCompPtr->GetPosition();
        const auto& yOffset = collider->GetShape()->GetAABB().getSize().y;
        const auto& scorePos = sf::Vector2f{ pos.x, pos.y - yOffset };
        GameManager::SpawnPointsText(scorePos, m_PointString);
        return;
    }

    if (diji::Helpers::isZero(hitInfo.normal.x))
        return;

    m_Speed = -m_Speed;
}

void thomasWasLate::MushroomScript::HandleBumpedBehavior(const bool IsBumpingLeft, const bool)
{
    const sf::Vector2f impulse = IsBumpingLeft ? sf::Vector2f{-200.f, -1000.f} : sf::Vector2f{200.f, -1000.f};
    m_ColliderCompPtr->ApplyImpulse(impulse);
    m_Speed = IsBumpingLeft ? -std::abs(m_Speed) : std::abs(m_Speed);
}

void thomasWasLate::MushroomScript::PlayStartAnimation()
{
    const auto& timelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = timelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.8f, .value= -51.f } };

    diji::Transform* transformPtr = GetOwner()->GetComponent<diji::Transform>();
    sf::Vector2f originalPos = transformPtr->GetPosition();

    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetPosition(originalPos.x, originalPos.y + y);
    };

    auto& [eventName, eventKeysVec] = timelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.8f, .callback= [&]()
            {
                const auto collider = GetOwner()->GetComponent<diji::Collider>();
                collider->SetCollisionResponse(diji::Collider::CollisionResponse::Block);
                collider->SetGenerateHitEvents(true);
                collider->SetRestitution(0.f);
                collider->SetMass(0.89f);
                collider->SetStaticFriction(0.25f);
                collider->SetAffectedByGravity(true);
                m_Paused = false;
            }
        }
    };

}
