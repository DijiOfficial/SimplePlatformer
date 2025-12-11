#pragma once
#include "Engine/Components/Component.h"
#include <SFML/Graphics/Rect.hpp>

namespace diji
{
    class Transform;
    class Camera;
    class Collider;
}

namespace superMarioBros
{
    class PlayerCharacter;
    class CameraClamping final : public diji::Component
    {
    public:
        explicit CameraClamping(diji::GameObject* ownerPtr) : Component(ownerPtr) {}
        ~CameraClamping() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override;

        void OnDisable() override {}
        void OnDestroy() override {}

    private:
        PlayerCharacter* m_PlayerCharacterCompPtr = nullptr;
        diji::Transform* m_PlayerTransformCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
        diji::Camera* m_CameraPtr = nullptr;
        sf::FloatRect m_Arena;
        sf::Vector2f m_LastPosition;
        float m_LastArenaPosX = 0.f;
        float m_PlayerHalfWidth = 0.f;
    };
}
