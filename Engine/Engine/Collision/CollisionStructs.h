#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace diji
{
    class Collider;

    enum class EventType : uint8_t
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

        sf::FloatRect otherAABB;

        bool hasHitEvent = false;
        const Collider* trigger;
        const Collider* other;
    };

    struct Prediction
    {
        Collider* collider;
        sf::FloatRect AABB;
        sf::Vector2f pos;
        sf::Vector2f vel;
        std::vector<CollisionInfo> collisionInfoVec;
    };

    struct SleepingCollider
    {
        Collider* collider;
        Prediction prediction;
    };

    struct SleepingColliderHash
    {
        using is_transparent = void;

        size_t operator()(const Collider* c) const noexcept
        {
            return std::hash<const Collider*>{}(c);
        }

        size_t operator()(const SleepingCollider& s) const noexcept
        {
            return std::hash<const Collider*>{}(s.collider);
        }
    };

    struct SleepingColliderEqual
    {
        using is_transparent = void;

        bool operator()(const Collider* a, const Collider* b) const noexcept
        {
            return a == b;
        }

        bool operator()(const SleepingCollider& a, const Collider* b) const noexcept
        {
            return a.collider == b;
        }

        bool operator()(const Collider* a, const SleepingCollider& b) const noexcept
        {
            return a == b.collider;
        }

        bool operator()(const SleepingCollider& a, const SleepingCollider& b) const noexcept
        {
            return a.collider == b.collider;
        }
    };

    struct RaycastHit
    {
        const Collider* collider = nullptr;
        CollisionInfo info;
        float distance = 0.f;           // from origin along ray direction
    };

    struct StaticColliderInfo
    {
        sf::FloatRect aabb;
        Collider* collider; // perhaps this can be optimized for memory usage if needed
    };

    enum class SleepState : uint8_t
    {
        Awake,
        Sleeping,
        PendingSleep,
        PendingWake
    };

    // Various optimizations and helper structs can go here
    struct PairHash // for quad tree collider pair hashing
    {
        size_t operator()(const std::pair<const Collider*, const Collider*>& p) const noexcept
        {
            return std::hash<const Collider*>{}(p.first) ^ std::hash<const Collider*>{}(p.second);
        }
    };
}
