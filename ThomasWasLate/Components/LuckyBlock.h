#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Timeline;
}

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class PlayerStates;

    class LuckyBlock final : public diji::Component
    {
    public:
        explicit LuckyBlock(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~LuckyBlock() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo) override;
        
    private:
        diji::Transform* m_TransformCompPtr = nullptr;
        sf::Vector2f m_OriginalPos = { 0.f, 0.f };
        
        bool m_IsHit = false;

        // temp?
        bool m_IsPlaying = false;
        bool m_SwitchedDirection = false;
        float m_PositionOffset = 0.f;
        float m_PositionOffsetSpeed = -200.f;

        void PlayAnimation();
        void CreateTimeline() const;
    };
}
