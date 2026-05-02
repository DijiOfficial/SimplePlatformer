#pragma once
#include "BaseEnemy.h"
#include "../../Interfaces/IShoveable.h"
#include "Engine/Singleton/TimerManager.h"

namespace superMarioBros
{
    class KoopaTroopa final : public BaseEnemy, public IShoveable
    {
    public:
        explicit KoopaTroopa(diji::GameObject* ownerPtr) : BaseEnemy{ ownerPtr } {}
        ~KoopaTroopa() noexcept override = default;

        void Start() override;

        void HandleStomp(const diji::Collider* other, const std::string& score) override;
        void OnHitEvent(const diji::Collider* other, const diji::CollisionInfo& hitInfo) override;
        void OnTriggerExit(const diji::Collider* other, const diji::CollisionInfo&) override;
        void OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo&) override;

        void HandleBumpedBehavior(bool isBumpingLeft, const bool addPoints = true) override;
        void Kill(bool isBumpingLeft, const bool addPoints = true) override;
        void Shove(bool isShovingLeft) override;

        bool IsStomped() const { return m_KoopaTroopaState == KoopaTroopaState::Stomped; }
    private:
        diji::TimerManager::TimerHandle m_TimerHandle{ std::numeric_limits<std::size_t>::max() };
        diji::Collider* m_EnemyColliderCompPtr = nullptr;
        const int BASE_COMBO_INDEX = 4;
        int m_CurrentComboIndex = BASE_COMBO_INDEX;
        float m_ShellSpeed = 1200.f;

        enum class KoopaTroopaState : uint8_t
        {
            Alive,
            Stomped,
            Bumped
        };
        KoopaTroopaState m_KoopaTroopaState = KoopaTroopaState::Alive;
        
        void HandleStomp();
        void HandleBumped();
        void HandleStopBumpMovement();

        void SetRespawnTimer();
        void SetShellAppearance() const;
    };
}
