#pragma once
#include <SFML/System/Vector2.hpp>

#include "Engine/Components/Component.h"
#include "Engine/Singleton/TimeSingleton.h"

namespace diji
{
    class Transform;
    class Camera;
    class Collider;
}

namespace thomasWasLate
{
    class PlayerCharacter final : public diji::Component
    {
    public:
        explicit PlayerCharacter(diji::GameObject* ownerPtr, float jumpTime);
        ~PlayerCharacter() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override;
        void FixedUpdate() override;
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void Move(const sf::Vector2f& direction) const;
        void Jump(bool ignoreTimer = false);
        void ClearJump();

        void SetMass(const float mass) { m_Mass = mass; }
        void ApplyForce(const sf::Vector2f& force) { m_NetForce += force; }
        void ApplyImpulse(const sf::Vector2f& impulse) { m_Velocity += impulse / m_Mass; }
        
    private:
        // TODO: like in Bullet, if speed is low enough for a certain time ignore physics calculations
        //temp
        diji::Camera* m_Camera = nullptr;
        //end temp
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
        sf::Vector2f m_SpawnPoint = { 0.f, 0.f };
        sf::Vector2f m_Velocity = { 0.f, 0.f };
        sf::Vector2f m_Acceleration = { 0.f, 0.f };
        sf::Vector2f m_LastPosition = { 0.f, 0.f };
        sf::Vector2f m_NewPosition = { 0.f, 0.f };

        sf::Vector2f m_NetForce = { 0.f, 0.f };
        sf::Vector2f m_Normal = { 0.f, 0.f };
        sf::Vector2f m_PredictedVelocity = { 0.f, 0.f };
        sf::Vector2f m_PredictedPosition = { 0.f, 0.f };
        float m_Speed = 400.f;
        float m_Mass = 1.f;
        const float GRAVITY = 981.f;
        const diji::TimeSingleton& m_TimeSingletonInstance = diji::TimeSingleton::GetInstance();
        // bool m_IsOnGround = false;
        // bool m_IsJumping = false;
        // const float GRAVITY = 300.f;
        // float m_JumpTime = 0.0f;
        float m_MaxJumpTime = 0.25f;

        void OnNewLevelLoaded();

        //Physics helpers
        // Structure to hold collision information
        struct CollisionInfo
        {
            sf::Vector2f point;          // Contact point
            sf::Vector2f normal;         // Collision normal (pointing away from surface)
            float penetration;           // How much objects overlap todo: maybe not necessary
            mutable float normalImpulse; // Impulse magnitude (calculated during resolution) todo: maybe not necessary
            sf::Vector2f tangent;        // Tangent vector for friction calculation todo: maybe not necessary
            bool hasCollision;           // Whether collision occurred
        };
        
        void PredictMovement();
        [[nodiscard]] std::vector<CollisionInfo> DetectCollision() const; // auto collision = DetectCollision()
        // resolve
        // for collision in collisions { resolve(col) -> apply friction(col) }
        void ResolveCollision(const CollisionInfo& collision);
        void ApplyFriction(const CollisionInfo& collision);

        void UpdateFinalPosition();
        
        // void FixedUpdate() {
        //     // Phase 1: Predict movement
        //     PredictMovement(dt);
        //
        //     // Phase 2: Detect collisions using predicted positions
        //     auto collisions = DetectCollisions();
        //
        //     // Phase 3: Resolve collisions and apply friction
        //     for (const auto& collision : collisions) {
        //         ResolveCollision(collision);
        //         ApplyFriction(collision);
        //     }
        //
        //     // Phase 4: Update final state
        //     UpdateFinalPositions(dt);
        // }
    };
}
