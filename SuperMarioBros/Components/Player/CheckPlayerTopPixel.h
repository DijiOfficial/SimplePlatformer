#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace superMarioBros
{
    class PlayerCharacter;
    class PlayerInputManager;

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
        PlayerInputManager* m_PlayerInputManagerCompPtr = nullptr;
        PlayerCharacter* m_PlayerCharacterCompPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
    };
}
