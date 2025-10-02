#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Sprite;
}

namespace thomasWasLate
{
    class BackgroundHandler final : public diji::Component
    {
    public:
        explicit BackgroundHandler(diji::GameObject* ownerPtr) : Component(ownerPtr) {}
        ~BackgroundHandler() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

    private:
        diji::Sprite* m_BackgroundSprite = nullptr;

        void OnNewLevelLoaded() const;
    };
}
