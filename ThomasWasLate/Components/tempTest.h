#pragma once
#include "Engine/Collision/Collider.h"
#include "Engine/Components/Component.h"

namespace thomasWasLate
{
    class tempTest final : public diji::Component
    {
    public:
        explicit tempTest(diji::GameObject* ownerPtr) : Component(ownerPtr) {}
        ~tempTest() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override;
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void SpawnRandomCollider();
        void SetMousePos(const sf::Vector2i& pos) { m_MousePos = static_cast<sf::Vector2f>(pos); }

        diji::Event<> OnBoxSpawnedEvent;

    private:
        sf::Vector2f m_MousePos;
        diji::Collider* m_ColliderPtr = nullptr;
        float m_Angle = 0.f;
    };
}
