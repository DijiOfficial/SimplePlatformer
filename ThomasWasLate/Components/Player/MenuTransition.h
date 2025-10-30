#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class MenuTransition final : public diji::Component
    {
    public:
        explicit MenuTransition(diji::GameObject* ownerPtr) : Component(ownerPtr) {}
        ~MenuTransition() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {} 
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

    private:
        diji::Transform* m_TransformCompPtr = nullptr;
    };
}
