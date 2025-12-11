#pragma once
#include "Engine/Components/Component.h"
#include "../../../Interfaces/IBumpable.h"

namespace superMarioBros
{
    class StaticCoin final : public diji::Component, public IBumpable
    {
    public:
        explicit StaticCoin(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~StaticCoin() noexcept override = default;

        void Init() override {}
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnTriggerEnter(const diji::Collider*, const diji::CollisionInfo&) override;
        void HandleBumpedBehavior(bool, bool) override;

    private:
        bool m_IsCollected = false;
    };
}
