#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace superMarioBros
{
    class PointsBehaviour final : public diji::Component
    {
    public:
        explicit PointsBehaviour(diji::GameObject* ownerPtr, const bool shouldIgnoreLifetime = false) : Component{ ownerPtr }, m_ShouldIgnoreLifetime{ shouldIgnoreLifetime } {}
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
        void SetSpeed(const float speed) { m_Speed = speed; }
        void SetMaxHeight(const float maxHeight) { m_MaxHeight = maxHeight; }

    private:
        diji::Transform* m_TransformCompPtr = nullptr;
        float m_MaxHeight = -10000.f;
        float m_Speed = -90.f;
        bool m_ShouldIgnoreLifetime = false;
    };
}
