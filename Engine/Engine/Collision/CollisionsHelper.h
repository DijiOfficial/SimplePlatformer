#pragma once
#include "PhysicsWorld.h"

namespace sf
{
    class CircleShape;
    class RectangleShape;
}

namespace diji
{
    class CollisionsHelper final
    {
    public:
        static bool ProcessCircleToCircleCollision(const sf::CircleShape& circleA, const sf::CircleShape& circleB, std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecA, std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecB, bool isCheckingOverlap);
        static bool ProcessCircleToBoxCollision(const sf::CircleShape& circleA, const sf::RectangleShape& rect, std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecA, std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecB, bool isCheckingOverlap);
        static bool ProcessBoxToBoxCollision(const sf::RectangleShape& rectA, const sf::RectangleShape& rectB, std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecA, std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecB, bool isCheckingOverlap);
        
    private:
        static std::vector<sf::Vector2f> GetBoxCorners(const sf::RectangleShape& rect);
        static std::vector<sf::Vector2f> GetBoxAxes(const std::vector<sf::Vector2f>& corners);
        static sf::Vector2f FindClosestPointToCircle(const sf::CircleShape& circle, const std::vector<sf::Vector2f>& corners);
        static void ProjectOntoAxis(const std::vector<sf::Vector2f>& points, const sf::Vector2f& axis, float& min, float& max);
        static void ProjectCircleOntoAxis(const sf::CircleShape& circle, const sf::Vector2f& axis, float& min, float& max);
        static sf::Vector2f GetCenterOfMass(const std::vector<sf::Vector2f>& points);
    };
}
