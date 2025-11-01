#include "BreakableBlock.h"
#include "../Others/Debris.h"
#include "../../../Singletons/GameManager.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Collision/Collider.h"

#include <array>

void thomasWasLate::BreakableBlock::Init()
{
    BaseBlock::Init();

    m_ShouldSwitchOnHit = false;
}

void thomasWasLate::BreakableBlock::OnAnimationStart()
{
    if (GameManager::GetInstance().GetCurrentPlayerState() == PlayerHealthState::Small) return;
    
    const sf::Vector2f& center = GetOwner()->GetComponent<diji::Transform>()->GetPosition();
    constexpr float offset = 15.f;

    const std::array offsets =
    {
        sf::Vector2f{-offset, -offset},
        sf::Vector2f{+offset, -offset},
        sf::Vector2f{-offset, +offset},
        sf::Vector2f{+offset, +offset}
    };

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
    
    Destroy();
    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(50);
}

