#include "MarioHelpers.h"

#include "../Components/Other/Coins/SmallCoinScript.h"
#include "../Interfaces/IBumpable.h"
#include "../Singletons/GameManager.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Singleton/SceneManager.h"

const std::vector<int> mario::MarioHelpers::s_StompPointsTable =
{
    100,   // 1st stomp
    200,   // 2nd stomp  
    400,   // 3rd stomp
    500,   // 4th stomp
    800,   // 5th stomp
    1000,  // 6th stomp
    2000,  // 7th stomp
    4000,  // 8th stomp
    5000,  // 9th stomp
    8000   // 10th stomp
    // 11th+ stomps give 1-Up (handled separately)
};

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
    const bool noHit = std::abs(coordAlongTangent) > halfExtentAlongTangent;
    if (!noHit)
        diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_bump.wav", false);
    
    return noHit;
}

void mario::MarioHelpers::CheckForCollisionAboveBlock(const diji::Collider* collider)
{
    const sf::Vector2f origin = collider->GetPosition();
    const sf::Vector2f dir = { 0, -1 };
    constexpr float offset = 23.f;
    const sf::Vector2f TopLeft = { origin.x - offset, origin.y - offset };
    const sf::Vector2f TopRight = { origin.x + offset, origin.y - offset };
    
    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(TopLeft, dir, 15.f, collider))
    {
        if (hit->info.hasCollision && (hit->collider->GetTag() == "enemy" || hit->collider->GetTag() == "powerUp" || hit->collider->GetTag() == "coin"))
            diji::InterfaceRegistry::GetInterface<thomasWasLate::IBumpable>(hit->collider->GetParent())->HandleBumpedBehavior(true);

    }

    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(TopRight, dir, 15.f, collider))
    {
        if (hit->info.hasCollision && (hit->collider->GetTag() == "enemy" || hit->collider->GetTag() == "powerUp" || hit->collider->GetTag() == "coin"))
            diji::InterfaceRegistry::GetInterface<thomasWasLate::IBumpable>(hit->collider->GetParent())->HandleBumpedBehavior(true);
    }
}

std::string mario::MarioHelpers::GetStompPointsAsString(const int bounceMultiplier)
{
    // Clamp to valid range (1-based index)
    const int index = bounceMultiplier - 1;
    
    if (index < 0 || index >= static_cast<int>(s_StompPointsTable.size()))
    {
        thomasWasLate::GameManager::GetInstance().AddLife();
        return "1UP";
    }

    thomasWasLate::GameManager::GetInstance().OnScoreAddedEvent.Broadcast(s_StompPointsTable[index]);
    return std::to_string(s_StompPointsTable[index]);
}

void mario::MarioHelpers::SpawnCoinAboveBlock(const sf::Vector2f& colliderCenterPos)
{
    auto coinTest = std::make_unique<diji::GameObject>();
    coinTest->AddComponents<diji::Transform>(600, 0);
    coinTest->AddComponents<diji::SpriteRenderComponent>("graphics/smallCoins.png", sf::Vector2i{ 25,50 }, 4, 0.03f);
    coinTest->AddComponents<thomasWasLate::SmallCoinScript>();
    diji::SceneManager::GetInstance().SpawnGameObject("G_SmallCoin", std::move(coinTest), { colliderCenterPos.x, colliderCenterPos.y - 50.f });
}