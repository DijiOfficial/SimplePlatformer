#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Singleton/TimeSingleton.h"

namespace diji
{
    class Transform;
    class SpriteRenderComponent;
}

namespace superMarioBros
{
    class PlayerCharacter;
    class PlayerStates;
    
    class PlayerInputManager final : public diji::Component
    {
    public:
        explicit PlayerInputManager(diji::GameObject* ownerPtr, float jumpTime);
        ~PlayerInputManager() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
            
        void Update() override;
        void FixedUpdate() override;
        void LateUpdate() override;

        void OnDisable() override {}
        void OnDestroy() override {}

        void SetAgainstCameraEdge(const bool isAgainst) { m_IsAgainstCameraEdge = isAgainst; }
        void Move(const sf::Vector2f& direction);
        void StopMove();
        void Jump();
        void ClearJump();
        void Sprint();
        void StopSprint();
        void Attack();
        void Crouch(bool isStart);
        void Bump();

    private:
        const diji::TimeSingleton& m_TimeSingletonInstance = diji::TimeSingleton::GetInstance();
        diji::SpriteRenderComponent* m_SpriteRenderCompPtr = nullptr;
        PlayerCharacter* m_PlayerCharacterCompPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;

        sf::Vector2f m_BaseMaxVelocity = { 350.f, 1000.f }; // const?
        sf::Vector2f m_SprintMaxVelocity = { 500.f, 1000.f };
        sf::Vector2f m_CurrSpeed = { 0.f, 0.f };

        enum class MovementDirection : uint8_t
        {
            Left,
            Right,
            None,
        };
        MovementDirection m_MovementDirection = MovementDirection::None;
        MovementDirection m_LookDirection = MovementDirection::Right;

        bool m_IsOnGround = false;
        bool m_IsJumping = false;
        bool m_IsCrouched = false;
        bool m_StoppedSprinting = false;
        bool m_CanJump = true;
        bool m_CanAttack = true;
        bool m_IsAgainstCameraEdge = false;

        float m_JumpForce = 950.f;
        float m_BaseAcceleration = 1000.f;
        float m_Acceleration = m_BaseAcceleration;
        float m_SprintAcceleration = 1500.f;
        float m_JumpTime = 0.0f;
        float m_MaxJumpTime = 0.25f;
        float m_MinJumpTime = 0.25f;
        float m_SprintDecelerationTimer = 0.f;
        float m_AttackFireballCooldownTimer = 0.1f;

        void DecelerateAfterSprint();
        void CheckIfPlayerIsGrounded();
    };
}
