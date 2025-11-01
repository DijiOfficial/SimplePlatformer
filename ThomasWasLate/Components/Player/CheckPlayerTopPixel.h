#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class PlayerCharacter;

    class CheckPlayerTopPixel final : public diji::Component
    {
    public:
        explicit CheckPlayerTopPixel(diji::GameObject* ownerPtr) : Component(ownerPtr){}
        ~CheckPlayerTopPixel() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

    private:
        diji::Transform* m_TransformCompPtr = nullptr;
        PlayerCharacter* m_PlayerCharacterCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
    };
}
