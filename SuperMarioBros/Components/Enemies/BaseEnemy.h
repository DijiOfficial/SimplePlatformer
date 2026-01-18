#pragma once
#include "Engine/Components/Component.h"
#include "../..//Interfaces/IBumpable.h"
#include "../..//Interfaces/IKillable.h"

#include <string>

namespace diji
{
    class Transform;
    class SpriteRenderComponent;
}

namespace superMarioBros
{
    class BaseEnemy : public diji::Component, public IBumpable, public IKillable
    {
    public:
        explicit BaseEnemy(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~BaseEnemy() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override;
        void FixedUpdate() override; 
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        virtual void HandleStomp(const diji::Collider* other, const std::string& score) = 0;
        void HandleBumpedBehavior(bool isBumpingLeft, const bool addPoints = true) override = 0;
        void Kill(bool isBumpingLeft, const bool addPoints = true) override = 0;
        void SetActivationMilestone(const int milestone) { m_ActivationMilestone = milestone; }

    protected:
        diji::SpriteRenderComponent* m_SpriteRenderCompPtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        const float BASE_SPEED = -200.f;
        float m_Speed = BASE_SPEED;
        int m_ActivationMilestone = -1;
        bool m_Paused = false;
        bool m_IsDead = false;

        void SetPauseState(const bool isPause) { m_Paused = isPause; }
        void CheckActivation(int milestone) const;
        void SpawnPointsText(const std::string& score) const;
        void Pause() { m_Paused = true; }
    };
}
