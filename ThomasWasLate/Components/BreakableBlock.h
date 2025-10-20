#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Timeline;
}

namespace thomasWasLate
{
    class PlayerStates;

    class BreakableBlock : public diji::Component
    {
    public:
        explicit BreakableBlock(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~BreakableBlock() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo) override;
        
    protected:
        diji::Timeline* m_TimelinePtr = nullptr;
        bool m_IsHit = false;

        virtual void CreateTimeline();
        
    private:
        void PlayAnimation() const;
    };
}
