#pragma once
#include "BaseEnemy.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/TimerManager.h"

namespace thomasWasLate
{
    class PiranhaPlant final : public BaseEnemy
    {
    public:
        explicit PiranhaPlant(diji::GameObject* ownerPtr) : BaseEnemy{ ownerPtr } {}
        ~PiranhaPlant() noexcept override = default;

        void Start() override;
        void Update() override;
        void FixedUpdate() override {}
        void HandleStomp(const diji::Collider*, const std::string&) override {}
        void OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo& hitInfo) override;
        void HandleBumpedBehavior(bool, const bool = true) override {}
        void OnDestroy() override;

        void Kill(bool isBumpingLeft, bool addPoints = true) override;

    private:
        diji::TimerManager::TimerHandle m_TimerHandle{ std::numeric_limits<std::size_t>::max() };
        diji::Transform* m_PlayerTransformCompPtr = nullptr;
        diji::Timeline* m_TimelinePtr = nullptr;
        bool m_IsEventTriggered = false;
        bool m_CanAttack = true;

        void CreateTimeLine();
        void PauseTimeline() const;
        void PauseTimeline(bool isPaused) const;
    };
}
