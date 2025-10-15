#include "BreakableBlock.h"
#include "Debris.h"
#include "../Helpers/MarioHelpers.h"
#include "../Singletons/GameManager.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Collision/Collider.h"

#include <array>

void thomasWasLate::BreakableBlock::Init()
{
    m_TimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());
    CreateTimeline();
}

void thomasWasLate::BreakableBlock::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (collider->GetTag() != "player" || hitInfo.normal.y >= 0.f || m_IsHit)
        return;

    const auto selfCollider = GetOwner()->GetComponent<diji::Collider>();
    const sf::Vector2f playerCenter = collider->GetPosition();
    const sf::FloatRect blockAABB = selfCollider->GetAABB();
    if (mario::MarioHelpers::DoesPlayerHitBottomOfBlock(playerCenter, blockAABB, hitInfo.normal)) return;

    m_IsHit = true;

    if (GameManager::GetInstance().GetCurrentPlayerState() == PlayerHealthState::Small)
    {
        m_TimelinePtr->PlayFromStart();
        mario::MarioHelpers::CheckForCollisionAboveBlock(selfCollider);

        return;
    }

    PlayAnimation();
}

void thomasWasLate::BreakableBlock::PlayAnimation() const
{
    // create particles
    const sf::Vector2f& center = GetOwner()->GetComponent<diji::Transform>()->GetPosition();
    constexpr float offset = 15.f;

    const std::array offsets =
    {
        sf::Vector2f{-offset, -offset},
        sf::Vector2f{+offset, -offset},
        sf::Vector2f{-offset, +offset},
        sf::Vector2f{+offset, +offset}
    };

    // helper lambda to create a particle with given debris index
    auto makeDebris = [] (int debrisIndex) -> std::unique_ptr<diji::GameObject>
    {
        auto obj = std::make_unique<diji::GameObject>();
        obj->AddComponents<diji::Transform>(0, 0);
        obj->AddComponents<diji::SpriteRenderComponent>("graphics/blockDebris.png", sf::Vector2i{24, 24}, 2, 0.13f);
        obj->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{24, 24});
        obj->GetComponent<diji::Collider>()->SetTag("debris");
        obj->GetComponent<diji::Collider>()->SetCollisionResponse(diji::Collider::CollisionResponse::Ignore);
        obj->AddComponents<Debris>(debrisIndex);
        return obj;
    };

    for (int i = 0; i < 4; ++i)
    {
        auto particle = makeDebris(i);
        diji::SceneManager::GetInstance().SpawnGameObject("debris", std::move(particle), center + offsets[i]);
    }
    
    diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(50);
}

void thomasWasLate::BreakableBlock::CreateTimeline()
{
    auto &track = m_TimelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.1f, .value= -20.f }, { .time= 0.2f, .value= 0.f } };
    
    diji::Transform* transformPtr = GetOwner()->GetComponent<diji::Transform>();
    sf::Vector2f originalPos = transformPtr->GetPosition();
    
    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetPosition(originalPos.x, originalPos.y + y);
    };

    m_TimelinePtr->Pause();

    auto& [eventName, eventKeysVec] = m_TimelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.2f, .callback= [&]()
            {
                m_IsHit = false;
                m_TimelinePtr->Pause();
            }
        }
    };
}
