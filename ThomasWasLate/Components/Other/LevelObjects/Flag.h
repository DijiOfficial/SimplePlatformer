#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Timeline;
}

namespace thomasWasLate
{
    class Flag final : public diji::Component
    {
    public:
        explicit Flag(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~Flag() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        diji::Event<> OnFlagAnimationFinishedEvent;

    private:
        diji::Timeline* m_TimelinePtr = nullptr;
        void StartAnimation();
    };
}
