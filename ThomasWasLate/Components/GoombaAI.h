#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class PlayerStates;

    class GoombaAI final : public diji::Component
    {
    public:
        // using Component::Component;
        explicit GoombaAI(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~GoombaAI() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

    private:
        diji::Collider* m_ColliderCompPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;

        float m_Speed = 400.f;
    };
}
