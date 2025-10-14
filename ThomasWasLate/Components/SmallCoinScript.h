#pragma once
#include "Engine/Components/Component.h"

namespace thomasWasLate
{
    class PlayerStates;

    class SmallCoinScript final : public diji::Component
    {
    public:
        explicit SmallCoinScript(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~SmallCoinScript() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

    private:
    };
}
