#pragma once
#include <string>
#include <SFML/System/Vector2.hpp>

#include "Engine/Components/Component.h"
#include "Engine/Singleton/TimeSingleton.h"
#include "PlayerStates.h"
#include "../../Interfaces/IPowerUp.h"

namespace diji
{
    class Timeline;
    class SpriteRenderComponent;
    class Transform;
    class Camera;
    class Collider;
}

namespace superMarioBros
{
    class PlayerStates;

    class PlayerCharacter final : public diji::Component, public IPowerUp
    {
    public:
        explicit PlayerCharacter(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~PlayerCharacter() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override;

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo&) override;
        void OnHitEvent(const diji::Collider* other, const diji::CollisionInfo&) override;

        void PausePlayer() { m_IsPaused = true; }
        void SetTransitionState();
        void KillPlayer() { OnFallingInHoleEvent.Broadcast(); HandleDeathSequence(); }
        void OnPowerUpCollected(PowerUpType power) override;

        template <typename T, typename... Args>
        void ChangeState(Args&&... args)
        {
            static_assert(std::is_base_of_v<PlayerStates, T>,"T must derive from PlayerStates");

            m_CurrentStateUPtr = std::make_unique<T>(std::forward<Args>(args)...);
            m_CurrentStateUPtr->OnEnter(GetOwner());
        }

        [[nodiscard]] PlayerStates::PlayerState GetCurrentState() const { return m_CurrentStateUPtr->GetState(); }
        [[nodiscard]] bool IsDead() const { return m_IsDead; }
        [[nodiscard]] bool IsPaused() const { return m_IsPaused; }
        [[nodiscard]] bool IsDeadOrPaused() const { return m_IsDead || m_IsPaused; }
        [[nodiscard]] bool IsSmallMario() const { return m_PowerUpState == PowerUpState::Small; }
        [[nodiscard]] bool IsFireMario() const { return m_PowerUpState == PowerUpState::Fire; }

        diji::Event<> OnHitByEnemyEvent;
        diji::Event<> OnFallingInHoleEvent;
        diji::Event<const diji::Collider*, const std::string&> OnEnemyStompedEvent;
        diji::Event<bool> OnPoweringUpEvent;
        diji::Event<> OnLevelFinishedEvent;
        diji::Event<> OnCastleReachedEvent;

    private:
        const diji::TimeSingleton& m_TimeSingletonInstance = diji::TimeSingleton::GetInstance();
        std::unique_ptr<PlayerStates> m_CurrentStateUPtr = nullptr;
        diji::SpriteRenderComponent* m_SpriteRenderCompPtr = nullptr;
        diji::Timeline* m_FlagPoleTimelinePtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;

        sf::Vector2f m_FlagCenter = { 0.f, 0.f };
        const sf::Vector2f UP_VECTOR = { 0.f, -1.f };

        enum class PowerUpState : uint8_t
        {
            Small = 0,
            Big,
            Fire,
        };
        PowerUpState m_PowerUpState = PowerUpState::Small;
        
        int m_BounceScoreMultiplier = 1;
        const float STOMP_THRESHOLD = 0.5f;
        const float DEATH_BUMP_STRENGTH = 950.f;
        
        float m_InvincibilityTimer = 0.f;
        float m_InvincibilityRenderTimer = 0.f;
        float m_StarPowerTimer = 0.0f;
        bool m_IsDead = false;
        bool m_IsPaused = false;
        bool m_IsInvincible = true;
        bool m_IsStartPoweredUp = false;
        bool m_IsFlagTriggered = false;
        bool m_KoopaStompToggle = false;

        void HandleDeathSequence();
        void PlayDeathSequence() const;
        void LoadPosition() const;
        void PlayGrowthAnimation();
        void PlayShrinkAnimation();
        void HandlePowerUpCollision();
        void InvisibilityFlash();
        void CheckEnemyStomp();
        void StompEnemy(const diji::Collider* other);
        void StompKoopa(const diji::Collider* other);
        void PlayFireTransitionAnimation();
        void HandleStarPickup();
        void UpdateStarPowerShader();
        void HandleLevelCompletion(const sf::Vector2f& center);
        void StopFlagAnimAndMoveToCastle();
        void CheckForSavedState();
        void HitByEnemy();
        bool IsValidStomp(const diji::Collider* other) const;
    };
}
