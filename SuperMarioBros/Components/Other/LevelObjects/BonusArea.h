#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Singleton/Helpers.h"

namespace superMarioBros
{
    class BonusArea final : public diji::Component
    {
    public:
        explicit BonusArea(diji::GameObject* ownerPtr, const diji::Directions entranceDirection) : Component{ ownerPtr }, m_BonusAreaEntranceDirection{ entranceDirection } {}
        ~BonusArea() noexcept override = default;

        void Init() override {}
        void OnEnable() override {}
        void Start() override;
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnTriggerEnter(const diji::Collider*, const diji::CollisionInfo&) override;
        void OnTriggerExit(const diji::Collider*, const diji::CollisionInfo&) override;

        void TryTriggerBonusArea();

    private:
        const diji::Collider* m_PlayerColliderPtr = nullptr;
        bool m_PlayerIsOnTarget = false;
        bool m_IsAlreadyTriggered = false;

        diji::Directions m_BonusAreaEntranceDirection = diji::Directions::None;

        [[nodiscard]] bool IsPlayerCenteredOnPipe() const;
    };
}
