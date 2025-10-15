#include "MarioHelpers.h"

#include "../Interfaces/IBumpable.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Collision/Collider.h"
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
