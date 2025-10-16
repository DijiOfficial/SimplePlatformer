#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class FireFlower final : public diji::Component
    {
    public:
        explicit FireFlower(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~FireFlower() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {} 
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnTriggerEnter(const diji::Collider* other) override;

    private:
        diji::Transform* m_TransformCompPtr = nullptr;

        void PlayStartAnimation() const;
    };
}
