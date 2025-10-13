#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class PlayerStates;

    class Debris final : public diji::Component
    {
    public:
        explicit Debris(diji::GameObject* ownerPtr, const int direction) : Component{ ownerPtr }, m_Direction{ direction } {}
        ~Debris() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void SetDirection(const int direction) { m_Direction = direction; }

    private:
        int m_Direction = 0;
        diji::Transform* m_TransformCompPtr = nullptr;
    };
}
