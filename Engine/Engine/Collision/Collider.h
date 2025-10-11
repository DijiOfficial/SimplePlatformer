#pragma once
#include "../Components/Component.h"
#include "CollisionShape.h"

#include <memory>
#include <stdexcept>

#include "../Singleton/TimeSingleton.h"

namespace diji 
{
    class Transform;
    
    class Collider final : public Component
    {
    public:
        explicit Collider(GameObject* ownerPtr) = delete;
        Collider() = delete;
        
        template <typename... Args>
        explicit Collider(GameObject* ownerPtr, const CollisionShape::ShapeType type, Args&&... args)
            : Component(ownerPtr), m_Type(type)
        {
            if constexpr (std::is_constructible_v<Circle, Args...>) // todo: look into how to fix other constructors being tested when using simple switch case
            {
                if (type == CollisionShape::ShapeType::CIRCLE)
                    m_Shape = std::make_unique<Circle>(std::forward<Args>(args)...);
            }
            if constexpr (std::is_constructible_v<Rect, Args...>)
            {
                if (type == CollisionShape::ShapeType::RECT)
                    m_Shape = std::make_unique<Rect>(std::forward<Args>(args)...);
            }
            if constexpr (std::is_constructible_v<Triangle, Args...>)
            {
                if (type == CollisionShape::ShapeType::TRIANGLE)
                    m_Shape = std::make_unique<Triangle>(std::forward<Args>(args)...);
            }

            if (!m_Shape)
                throw std::invalid_argument("Shape does not exist or invalid arguments for the specified shape type.");
        }

        ~Collider() noexcept override = default;

        void Init() override {}
        void OnEnable() override {}
        void Start() override;

        void FixedUpdate() override;
        void Update() override;
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override;

        [[nodiscard]] CollisionShape* GetShape() const { return m_Shape.get(); }

        void SetVelocity(const sf::Vector2f& vel);
        [[nodiscard]] sf::Vector2f GetVelocity() const { return m_Velocity; }
        
        void ApplyImpulse(const sf::Vector2f& impulse) { m_Velocity += impulse; }
        void ApplyForce(const sf::Vector2f& force) { m_NetForce += force; }
        void SetMass(const float mass) { m_Mass = mass; }
        [[nodiscard]] float GetMass() const { return m_Mass; }
        
        void SetStatic(const bool isStatic) { m_IsStatic = isStatic; }
        [[nodiscard]] bool IsStatic() const { return m_IsStatic; }
        
        void SetAffectedByGravity(const bool isAffected) { m_AffectedByGravity = isAffected; }
        [[nodiscard]] bool IsAffectedByGravity() const { return m_AffectedByGravity; }
        
        void SetPosition(const sf::Vector2f& pos) const;
        [[nodiscard]] sf::Vector2f GetPosition() const;

        void SetFriction(const float friction) { m_Friction = std::clamp(friction, 0.f, 1.f); }
        [[nodiscard]] float GetFriction() const { return m_Friction; }
        
        [[nodiscard]] sf::FloatRect GetAABB() const;
        [[nodiscard]] sf::FloatRect GetAABBAt(const sf::Vector2f& pos) const;

        void SetNewPosition(const sf::Vector2f& pos) { m_NewPosition = pos; }
        [[nodiscard]] sf::Vector2f GetNewPosition() const { return m_NewPosition; }

        void ClearNetForce() { m_NetForce = sf::Vector2f{ 0, 0 }; }
        [[nodiscard]] sf::Vector2f GetNetForce() const { return m_NetForce; }

        void SetRestitution(const float restitution) { m_Restitution = restitution; }
        [[nodiscard]] float GetRestitution() const { return m_Restitution; }

        void SetMaxVelocity(const sf::Vector2f& maxVel) { m_MaxVelocity = maxVel; }
        [[nodiscard]] sf::Vector2f GetMaxVelocity() const { return m_MaxVelocity; }

        void SetGenerateHitEvents(const bool generateHitEvent) { m_IsGenerateHitEvents = generateHitEvent; }
        [[nodiscard]] bool IsGenerateHitEvents() const { return m_IsGenerateHitEvents; }

        void IgnoreCollider(const Collider* collider);
        [[nodiscard]] bool IsIgnoringCollider(const Collider* collider) const;

        [[nodiscard]] CollisionShape::ShapeType GetShapeType() const { return m_Type; }
        [[nodiscard]] const GameObject* GetParent() const { return GetOwner(); }
        
        // not a fan of string tags perhaps use enums?
        void SetTag (const std::string& tag) { m_Tag = tag; }
        [[nodiscard]] const std::string& GetTag() const { return m_Tag; }

        enum class CollisionResponse
        {
            Ignore = 0, // todo: add support for ignore functionality
            Overlap,
            Block
        };
        
        void SetCollisionResponse(const CollisionResponse response) { m_CollisionResponse = response; }
        [[nodiscard]] CollisionResponse GetCollisionResponse() const { return m_CollisionResponse; }

    private:
        Transform* m_TransformCompPtr = nullptr;
        std::vector<const Collider*> m_IgnoredColliders;
        CollisionShape::ShapeType m_Type;
        std::unique_ptr<CollisionShape> m_Shape;
        const TimeSingleton& m_TimeSingletonInstance = TimeSingleton::GetInstance();

        // physics settings
        CollisionResponse m_CollisionResponse = CollisionResponse::Block;
        
        // physics state
        sf::Vector2f m_Velocity{0.f, 0.f};
        sf::Vector2f m_MaxVelocity{0.f, 0.f};
        sf::Vector2f m_NewPosition{ 0.f, 0.f };
        sf::Vector2f m_LastPosition{ 0.f, 0.f };
        sf::Vector2f m_NetForce{ 0.f, 0.f };
        float m_Mass = 1.f;
        float m_Friction = 0.5f; // [0,1]
        float m_Restitution = 1.f; // bounciness [0,1]
        bool m_IsStatic = false; // immovable object
        bool m_AffectedByGravity = true;
        bool m_IsGenerateHitEvents = false;
  
        std::string m_Tag = "Untagged";
    };
}

