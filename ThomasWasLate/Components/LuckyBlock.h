#pragma once
#include "Engine/Components/Component.h"

namespace thomasWasLate
{
    class LuckyBlock final : public diji::Component
    {
    public:
        explicit LuckyBlock(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~LuckyBlock() noexcept override = default;

        void Init() override {}
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo) override;
        void SetAsPowerUpBlock() { m_IsPowerUpBlock = true; }
        
    private:
        bool m_IsHit = false;
        bool m_IsPowerUpBlock = false;

        void PlayAnimation() const;
        void CreateTimeline() const;
        void SpawnPowerUp() const;
    };
}
