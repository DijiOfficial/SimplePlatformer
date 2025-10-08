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
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void Move(const sf::Vector2f& direction) const;
        void Jump();
        void ClearJump();
        void Sprint();
        void StopSprint();

    private:
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
        sf::Vector2f m_SpawnPoint = { 0.f, 0.f };
        const diji::TimeSingleton& m_TimeSingletonInstance = diji::TimeSingleton::GetInstance();

        sf::Vector2f m_CurrSpeed = { 0.f, 0.f };
        sf::Vector2f m_PreviousSpeed = { 0.f, 0.f };

        sf::Vector2f m_BaseMaxVelocity = { 400.f, 800.f };
        sf::Vector2f m_SprintMaxVelocity = { 800.f, 800.f };

        float m_JumpForce = 2000.f;
        float m_Acceleration = 500.f;
        float m_BaseAcceleration = 500.f;
        float m_SprintAcceleration = 900.f;
        bool m_IsOnGround = false;
        bool m_IsJumping = false;
        bool m_StoppedSprinting = false;
        float m_JumpTime = 0.0f;
        float m_MaxJumpTime = 0.25f;
        float m_MinJumpTime = 0.25f;
        float m_SprintDecelerationTimer = 0.f;

        void OnNewLevelLoaded();
    };
}
