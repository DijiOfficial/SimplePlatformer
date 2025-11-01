#pragma once
#include "Engine/Components/Component.h"

namespace thomasWasLate
{
    class CheckPoint final : public diji::Component
    {
    public:
        explicit CheckPoint(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~CheckPoint() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void SetActivationMilestone(const int milestone) { m_ActivationMilestone = milestone; }

    private:
        int m_ActivationMilestone = -1;
        
        void CheckActivation(int milestone) const;
    };
}
