#pragma once
#include <memory>
#include <set>
#include <string>
#include <SFML/System/Vector2.hpp>

#include "Engine/Components/Component.h"
#include "Engine/Singleton/TimeSingleton.h"
#include "PlayerStates.h"

namespace diji
{
    class SpriteRenderComponent;
    class Transform;
    class Camera;
    class Collider;
}

namespace thomasWasLate
{
    class PlayerStates;

    class PlayerCharacter final : public diji::Component
    {
    public:
        explicit PlayerCharacter(diji::GameObject* ownerPtr, float jumpTime);
        ~PlayerCharacter() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override;
        void LateUpdate() override;

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnTriggerEnter(const diji::Collider* other) override;
        void OnHitEvent(const diji::Collider* other, const diji::CollisionInfo&) override;

        void Move(const sf::Vector2f& direction);
        void StopMove();
        void Jump();
        void ClearJump();
        void Sprint();
        void StopSprint();

        diji::Event<> OnHitByEnemyEvent;
        diji::Event<> OnFallingInHoleEvent;
        diji::Event<> OnExtraLifeGainedEvent;
        diji::Event<int> OnPointsScoredEvent;
        diji::Event<const diji::Collider*, const std::string&> OnEnemyStompedEvent;
        diji::Event<bool> OnPoweringUpEvent;
        

    private:
        static const std::vector<int> s_StompPointsTable;
        std::unique_ptr<PlayerStates> m_CurrentStateUPtr = nullptr;
        const std::set<std::string> GROUND_TAGS = { "ground", "luckyBlock", "breakBlock" };
        diji::SpriteRenderComponent* m_SpriteRenderCompPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
        sf::Vector2f m_SpawnPoint = { 0.f, 0.f };
        const diji::TimeSingleton& m_TimeSingletonInstance = diji::TimeSingleton::GetInstance();
        const sf::Vector2f UP_VECTOR = { 0.f, -1.f };

        sf::Vector2f m_CurrSpeed = { 0.f, 0.f };

        sf::Vector2f m_BaseMaxVelocity = { 800.f, 1000.f };
        sf::Vector2f m_SprintMaxVelocity = { 1000.f, 1000.f };

        enum class MovementDirection : uint8_t
        {
            Left,
            Right,
            None,
        };
        MovementDirection m_MovementDirection = MovementDirection::None;

        enum class PowerUpState : uint8_t
        {
            Small,
            Big,
            Fire
        };
        PowerUpState m_PowerUpState = PowerUpState::Small;
        
        int m_BounceScoreMultiplier = 1;
        const float STOMP_THRESHOLD = 0.5f;
        float m_JumpForce = 2000.f;
        float m_BaseAcceleration = 1000.f;
        float m_Acceleration = m_BaseAcceleration;
        float m_SprintAcceleration = 1500.f;
        float m_JumpTime = 0.0f;
        float m_MaxJumpTime = 0.25f;
        float m_MinJumpTime = 0.25f;
        float m_SprintDecelerationTimer = 0.f;
        bool m_IsOnGround = false;
        bool m_IsJumping = false;
        bool m_StoppedSprinting = false;
        bool m_IsLookingLeft = false;
        bool m_IsDead = false;
        bool m_CanJump = true;
        bool m_IsPaused = false;

        void HandleDeathSequence();
        void PlayDeathSequence() const;
        void OnNewLevelLoaded();
        [[nodiscard]] std::string GetStompPointsAsString(int bounceMultiplier);
        void DecelerateAfterSprint();
        void CheckIfPlayerIsGrounded();
        void PlayGrowthAnimation();
        void HandlePowerUpCollision();
    };
}
