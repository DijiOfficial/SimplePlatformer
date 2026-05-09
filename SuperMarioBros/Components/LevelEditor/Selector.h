#pragma once
#include "Engine/Components/Component.h"

namespace superMarioBros
{
    class Selector final : public diji::Component
    {
    public:
        explicit Selector(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~Selector() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
            
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
    };
}
