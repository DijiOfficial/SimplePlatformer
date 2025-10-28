#pragma once
#include "../../Interfaces/IBumpable.h"
#include "../../Interfaces/IKillable.h"
#include "../../Interfaces/IShoveable.h"

#include <string>

#include "Engine/Singleton/TimerManager.h"

namespace diji
{
    class SpriteRenderComponent;
}

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class PlayerStates;

    class KoopaTroopa final : public IBumpable, public IKillable, public IShoveable
    {
    public:
        explicit KoopaTroopa(diji::GameObject* ownerPtr) : IBumpable{ ownerPtr } {}
        ~KoopaTroopa() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override; 
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void HandleStomp(const diji::Collider* other, const std::string& score);
        void OnHitEvent(const diji::Collider* other, const diji::CollisionInfo& hitInfo) override;
        void OnTriggerExit(const diji::Collider* other, const diji::CollisionInfo&) override;
        void OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo&) override;

        void HandleBumpedBehavior(bool isBumpingLeft, const bool addPoints = true) override;
        void SetActivationMilestone(const int milestone) { m_ActivationMilestone = milestone; }

        void Kill(bool isBumpingLeft, const bool addPoints = true) override;
        void Shove(bool isShovingLeft) override;

    private:
        diji::TimerManager::TimerHandle m_TimerHandle{ std::numeric_limits<std::size_t>::max() };
        diji::Collider* m_ColliderCompPtr = nullptr;
        diji::Collider* m_EnemyColliderCompPtr = nullptr;
        diji::SpriteRenderComponent* m_SpriteRenderCompPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        bool m_Paused = false;
        int m_ActivationMilestone = -1;
        const int BASE_COMBO_INDEX = 4;
        int m_CurrentComboIndex = BASE_COMBO_INDEX;

        enum class KoopaTroopaState : uint8_t
        {
            Alive,
            Stomped,
            Bumped
        };
        KoopaTroopaState m_KoopaTroopaState = KoopaTroopaState::Alive;
        
        const float BASE_SPEED = -400.f;
        float m_Speed = BASE_SPEED;
        float m_ShellSpeed = 1200.f;
        void SetPauseState(const bool isPause) { m_Paused = isPause; }
        void CheckActivation(int milestone) const;

        //new
        void HandleStomp();
        void HandleBumped();
        void HandleStopBumpMovement();

        void SpawnPointsText(const std::string& score) const;
        void SetRespawnTimer();
        void SetShellAppearance() const;
    };
}
