#pragma once
#include <string>
#include <vector>
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
        static const std::vector<int> s_StompPointsTable;
        static [[nodiscard]] bool DoesPlayerHitBottomOfBlock(const sf::Vector2f& playerCenter, const sf::FloatRect& blockAABB, const sf::Vector2f& normal);
        static void CheckForCollisionAboveBlock(const diji::Collider* collider);
        static [[nodiscard]] std::string GetStompPointsAsString(const int bounceMultiplier);
        
        static void SpawnCoinAboveBlock(const sf::Vector2f& colliderCenterPos);
    };
}
