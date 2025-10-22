#include "StartPower.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/SceneManager.h"
#include "../Player/PlayerCharacter.h"
#include "../../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Singleton/Helpers.h"

void thomasWasLate::StartPower::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_ColliderCompPtr->SetRestitution(1.5f);
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
    m_ColliderCompPtr->SetStaticFriction(0.f);
    m_ColliderCompPtr->SetMaxVelocity(sf::Vector2f{ 600.f, 800.f });
    m_ColliderCompPtr->SetTag("star");
    m_ColliderCompPtr->SetAffectedByGravity(false);
    PlayStartAnimation();
}

void thomasWasLate::StartPower::OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo& hitInfo)
{
    if (other->GetTag() == "ground")
    {
        const auto& velocity = m_ColliderCompPtr->GetVelocity() * 1.2f;
        if (diji::Helpers::isZero(hitInfo.normal.y))
            m_ColliderCompPtr->SetVelocity(sf::Vector2f{ velocity.x * hitInfo.normal.x, velocity.y });
        else
            m_ColliderCompPtr->SetVelocity(sf::Vector2f{ velocity.x, velocity.y * hitInfo.normal.y });

        return;
    }
    
    if (other->GetTag() != "player")
        return;
    
    Destroy();

    const auto& pos = m_TransformCompPtr->GetPosition();
    constexpr float yOffset = 50.f;
    const auto& scorePos = sf::Vector2f{ pos.x, pos.y - yOffset };
    GameManager::SpawnPointsText(scorePos, "1000");
}

void thomasWasLate::StartPower::PlayStartAnimation() const
{
    const auto& timelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = timelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.8f, .value= -50.f } };

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
                m_ColliderCompPtr->SetAffectedByGravity(true);
                m_ColliderCompPtr->ApplyImpulse(sf::Vector2f{ 300.f, -800.f });
            }
        }
    };
}
