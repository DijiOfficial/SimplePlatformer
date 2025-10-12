#pragma once
#include <SFML/System/Vector2.hpp>

namespace diji
{
    class Collider;

    enum class EventType
    {
        Enter = 0,
        Stay = 1,
        Exit = 2,
        Hit = 3
    };

    struct CollisionInfo
    {
        sf::Vector2f point;             // Contact point
        sf::Vector2f normal;            // Collision normal (pointing away from surface)
        float penetration;              // How much objects overlap
        mutable float normalImpulse;    // Impulse magnitude (calculated during resolution)
        sf::Vector2f tangent;           // Tangent vector for friction calculation
        bool hasCollision;              // Whether collision occurred
    };

    struct RaycastHit
    {
        const Collider* collider = nullptr;
        CollisionInfo info;
        float distance = 0.f;           // from origin along ray direction
    };
}
