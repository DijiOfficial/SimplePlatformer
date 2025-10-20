#include "MarioHelpers.h"

#include "../Components/Other/SmallCoinScript.h"
#include "../Interfaces/IBumpable.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/SceneManager.h"

bool mario::MarioHelpers::DoesPlayerHitBottomOfBlock(const sf::Vector2f& playerCenter, const sf::FloatRect& blockAABB, const sf::Vector2f& normal)
{
    const sf::Vector2f blockCenter{ blockAABB.left + blockAABB.width * 0.5f, blockAABB.top  + blockAABB.height * 0.5f };
    const sf::Vector2f halfExtents{ blockAABB.width * 0.5f, blockAABB.height * 0.5f };

    const sf::Vector2f normalizedNormal = diji::Helpers::Normalize(normal);
    const sf::Vector2f tangent{ -normalizedNormal.y, normalizedNormal.x };

    const sf::Vector2f rel = playerCenter - blockCenter;
    const float coordAlongTangent = rel.x * tangent.x + rel.y * tangent.y;
    const float halfExtentAlongTangent = std::abs(tangent.x) * halfExtents.x + std::abs(tangent.y) * halfExtents.y + 8.f;

    // is block above player center
    return std::abs(coordAlongTangent) > halfExtentAlongTangent;
}

void mario::MarioHelpers::CheckForCollisionAboveBlock(const diji::Collider* collider)
{
    const sf::Vector2f origin = collider->GetPosition();
    const sf::Vector2f dir = { 0, -1 };
    constexpr float offset = 23.f;
    const sf::Vector2f TopLeft = { origin.x - offset, origin.y - offset };
    const sf::Vector2f TopRight = { origin.x + offset, origin.y - offset };
    
    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(TopLeft, dir, 10.f, collider))
    {
        if (hit->info.hasCollision && (hit->collider->GetTag() == "enemy" || hit->collider->GetTag() == "powerUp"))
            hit->collider->GetParent()->GetComponent<thomasWasLate::IBumpable>()->HandleBumpedBehavior(true);
    }

    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(TopRight, dir, 10.f, collider))
    {
        if (hit->info.hasCollision && (hit->collider->GetTag() == "enemy" || hit->collider->GetTag() == "powerUp"))
            hit->collider->GetParent()->GetComponent<thomasWasLate::IBumpable>()->HandleBumpedBehavior(false);
    }
}

void mario::MarioHelpers::SpawnCoinAboveBlock(const sf::Vector2f& colliderCenterPos)
{
    auto coinTest = std::make_unique<diji::GameObject>();
    coinTest->AddComponents<diji::Transform>(600, 0);
    coinTest->AddComponents<diji::SpriteRenderComponent>("graphics/smallCoins.png", sf::Vector2i{ 25,50 }, 4, 0.03f);
    coinTest->AddComponents<thomasWasLate::SmallCoinScript>();
    diji::SceneManager::GetInstance().SpawnGameObject("G_SmallCoin", std::move(coinTest), { colliderCenterPos.x, colliderCenterPos.y - 50.f });
}
