#include "MushroomScript.h"

#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/SceneManager.h"
#include "PlayerCharacter.h"
#include "../Singletons/GameManager.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Singleton/TimeSingleton.h"

void thomasWasLate::MushroomScript::Init()
{
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
        diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
}

void thomasWasLate::MushroomScript::FixedUpdate()
{
    if (m_Paused) return;
    
    m_TransformCompPtr->AddOffset(m_Speed * diji::TimeSingleton::GetInstance().GetFixedUpdateDeltaTime(), 0.f);
}

void thomasWasLate::MushroomScript::OnTriggerEnter(const diji::Collider* other)
{
    if (other->GetTag() == "player")
        diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
}

void thomasWasLate::MushroomScript::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (m_Paused) return;
    if (collider->GetTag() == "player")
    {
        diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
        const auto& pos = m_TransformCompPtr->GetPosition();
        const auto& yOffset = collider->GetShape()->GetAABB().getSize().y;
        const auto& scorePos = sf::Vector2f{ pos.x, pos.y - yOffset };
        GameManager::SpawnPointsText(scorePos, "1000");
        return;
    }

    if (diji::Helpers::isZero(hitInfo.normal.x))
        return;

    m_Speed = -m_Speed;
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
                collider->SetTag("powerUp");
                collider->SetRestitution(0.f);
                collider->SetMass(0.89f);
                collider->SetStaticFriction(0.25f);
                collider->SetAffectedByGravity(true);
                m_Paused = false;
            }
        }
    };

}
