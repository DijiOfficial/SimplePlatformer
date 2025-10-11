#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class PointsBehaviour final : public diji::Component
    {
    public:
        explicit PointsBehaviour(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~PointsBehaviour() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void SetValue(int value) const;

    private:
        diji::Transform* m_TransformCompPtr = nullptr;
        float m_Speed = -130.f;
    };
}
