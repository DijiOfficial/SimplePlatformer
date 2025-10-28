#pragma once
#include "../Components/Component.h"
#include "CollisionShape.h"
#include "../Singleton/TimeSingleton.h"

#include <memory>
#include <stdexcept>

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
        
        // void SetColliderPosition(const sf::Vector2f& pos);
        [[nodiscard]] sf::Vector2f GetPosition() const;

        void SetKineticFriction(const float friction) { m_KineticFriction = std::clamp(friction, 0.f, 1.f); }
        [[nodiscard]] float GetKineticFriction() const { return m_StaticFriction; }

        void SetStaticFriction(const float friction) { m_StaticFriction = std::clamp(friction, 0.f, 1.f); }
        [[nodiscard]] float GetStaticFriction() const { return m_KineticFriction; }
        
        [[nodiscard]] sf::FloatRect GetAABB() const;
        [[nodiscard]] sf::FloatRect GetAABBAt(const sf::Vector2f& pos) const;

        void SetNewPosition(const sf::Vector2f& pos) { m_LastPosition = m_NewPosition; m_NewPosition = pos; }
        [[nodiscard]] sf::Vector2f GetNewPosition() const { return m_NewPosition; }

        void ClearNetForce() { m_NetForce = sf::Vector2f{ 0, 0 }; }
        [[nodiscard]] sf::Vector2f GetNetForce() const { return m_NetForce; }

        void SetRestitution(const float restitution) { m_Restitution = restitution; }
        [[nodiscard]] float GetRestitution() const { return m_Restitution; }

        void SetMaxVelocity(const sf::Vector2f& maxVel) { m_MaxVelocity = maxVel; }
        [[nodiscard]] sf::Vector2f GetMaxVelocity() const { return m_MaxVelocity; }

        void SetGenerateHitEvents(const bool generateHitEvent) { m_IsGenerateHitEvents = generateHitEvent; }
        [[nodiscard]] bool IsGenerateHitEvents() const { return m_IsGenerateHitEvents; }

        void SetIsMoveable(const bool isMoveable) { m_IsMoveable = isMoveable; }
        [[nodiscard]] bool IsMoveable() const { return m_IsMoveable; }

        void IgnoreCollider(const Collider* collider);
        [[nodiscard]] bool IsIgnoringCollider(const Collider* collider) const;

        void OverlapCollider(const Collider* collider);
        [[nodiscard]] bool IsOverlappingCollider(const Collider* collider) const;
        void ClearOverlappedCollider(const Collider* collider);
        void ClearAllOverlappedCollider();

        void SetActive(const bool isActive) { m_IsActive = isActive; }
        [[nodiscard]] bool IsActive() const { return m_IsActive; }

        [[nodiscard]] CollisionShape::ShapeType GetShapeType() const { return m_Type; }
        [[nodiscard]] const GameObject* GetParent() const { return GetOwner(); }

        void ResizeCollider(const sf::Vector2f& size) const;
        void ResizeCollider(float radius) const;
        
        // not a fan of string tags perhaps use enums?
        void SetTag (const std::string& tag) { m_Tag = tag; }
        [[nodiscard]] const std::string& GetTag() const { return m_Tag; }

        enum class CollisionResponse : uint8_t
        {
            Ignore = 0,
            Overlap,
            Block
        };
        
        void SetCollisionResponse(const CollisionResponse response) { m_CollisionResponse = response; }
        [[nodiscard]] CollisionResponse GetCollisionResponse() const { return m_CollisionResponse; }

        void SetIgnoreAllDynamicColliders(const bool ignore) { m_IsIgnoringDynamicColliders = ignore; }
        [[nodiscard]] bool IsIgnoringAllDynamicColliders() const { return m_IsIgnoringDynamicColliders; }
        
        [[nodiscard]] sf::Vector2f GetSurfaceNormalAt(const sf::Vector2f& point) const;

        void ValidateColliderLists();

        // temp
        bool IsOverlapEmpty() const { return m_CollidersToOverlap.empty(); }
        
    private:
        // todo: if velocity is zero for a certain amount of time, set similar to static to save calculations
        Transform* m_TransformCompPtr = nullptr;
        std::vector<const Collider*> m_IgnoredColliders;
        std::vector<const Collider*> m_CollidersToOverlap;
        CollisionShape::ShapeType m_Type;
        std::unique_ptr<CollisionShape> m_Shape;
        const TimeSingleton& m_TimeSingletonInstance = TimeSingleton::GetInstance();

        // physics settings
        CollisionResponse m_CollisionResponse = CollisionResponse::Block;
        
        // physics state
        sf::Vector2f m_Velocity{0.f, 0.f};
        sf::Vector2f m_MaxVelocity{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        sf::Vector2f m_NewPosition{ 0.f, 0.f };
        sf::Vector2f m_LastPosition{ 0.f, 0.f };
        sf::Vector2f m_NetForce{ 0.f, 0.f };
        float m_Mass = 1.f;
        float m_KineticFriction = 0.5f; // [0,1]
        float m_StaticFriction = 0.5f; // [0,1]
        float m_Restitution = 1.f; // bounciness [0,1]
        bool m_IsStatic = false; // immovable object
        bool m_AffectedByGravity = true;
        bool m_IsGenerateHitEvents = false;
        bool m_IsMoveable = true;
        bool m_IsIgnoringDynamicColliders = false; // todo: this should be like Unreal collision settings instead
        bool m_IsActive = true;
  
        std::string m_Tag = "Untagged";
    };
}

