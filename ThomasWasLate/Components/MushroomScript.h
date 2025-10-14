#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class PlayerStates;

    class MushroomScript final : public diji::Component
    {
    public:
        explicit MushroomScript(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~MushroomScript() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override; 
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnTriggerEnter(const diji::Collider* other) override;
        void OnHitEvent(const diji::Collider*, const diji::CollisionInfo& hitInfo) override;

    private:
        diji::Transform* m_TransformCompPtr = nullptr;
        bool m_Paused = true;

        float m_Speed = 400.f;

        void PlayStartAnimation();
    };
}
