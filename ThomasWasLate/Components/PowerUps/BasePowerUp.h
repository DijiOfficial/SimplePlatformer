#pragma once
#include "Engine/Components/Component.h"
#include "../../Interfaces/IPowerUp.h"

#include <string>

namespace diji
{
    class Transform;
    class Timeline;
}

namespace thomasWasLate
{
    class BasePowerUp : public diji::Component
    {
    public:
        explicit BasePowerUp(diji::GameObject* ownerPtr, IPowerUp::PowerUpType powerUpType, bool canMove, std::string pointString = "1000");
        ~BasePowerUp() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override;
        void FixedUpdate() override;
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnTriggerEnter(const diji::Collider*, const diji::CollisionInfo&) override;
        void OnHitEvent(const diji::Collider*, const diji::CollisionInfo&) override;

    protected:
        diji::Timeline* m_TimelinePtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
        std::string m_PointString = "1000";
        IPowerUp::PowerUpType m_PowerUpType = IPowerUp::None;
        float m_Speed = 400.f;
        bool m_Paused = true;
        bool m_CanMove = false;
        
        void PlayStartAnimation();
        void OnPickup(const diji::Collider* other) const;
        virtual void OnAnimationComplete() {}
    };
}
