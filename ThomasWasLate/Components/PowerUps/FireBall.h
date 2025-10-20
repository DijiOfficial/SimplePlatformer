#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Camera;
}

namespace thomasWasLate
{
    class PlayerStates;

    class FireBall final : public diji::Component
    {
    public:
        explicit FireBall(diji::GameObject* ownerPtr, diji::Collider* owner, const bool isGoingRight);
        ~FireBall() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override;

        void OnDisable() override {}
        void OnDestroy() override;

        void OnHitEvent(const diji::Collider* other, const diji::CollisionInfo& hitInfo) override;

    private:
        diji::Camera* m_CameraPtr = nullptr;
        diji::Collider* m_PlayerCollider = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
        bool m_IsGoingRight = true;
    };
}
