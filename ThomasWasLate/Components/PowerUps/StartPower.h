#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class StartPower final : public diji::Component
    {
    public:
        explicit StartPower(diji::GameObject* ownerPtr) : Component(ownerPtr) {}
        ~StartPower() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {} 
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo& hitInfo) override;
        
    private:
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;

        void PlayStartAnimation() const;
    };
}
