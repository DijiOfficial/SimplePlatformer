#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace diji
{
    class Collider;
}

namespace mario
{
    
    class MarioHelpers
    {
    public:
        static bool DoesPlayerHitBottomOfBlock(const sf::Vector2f& playerCenter, const sf::FloatRect& blockAABB, const sf::Vector2f& normal);
        static void CheckForCollisionAboveBlock(const diji::Collider* collider);
        static void SpawnCoinAboveBlock(const sf::Vector2f& colliderCenterPos);
    };
}
