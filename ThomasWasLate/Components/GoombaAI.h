#pragma once
#include "../Interfaces/IBumpable.h"

#include <string>

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class PlayerStates;

    class GoombaAI final : public IBumpable
    {
    public:
        explicit GoombaAI(diji::GameObject* ownerPtr) : IBumpable{ ownerPtr } {}
        ~GoombaAI() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override; 
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void HandleStomp(const diji::Collider* other, const std::string& score);
        void OnHitEvent(const diji::Collider*, const diji::CollisionInfo& hitInfo) override;

        void HandleBumpedBehavior(bool IsBumpingLeft) override;

    private:
        diji::Collider* m_ColliderCompPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        bool m_Paused = false;

        float m_Speed = -400.f;
        void SetPauseState(const bool isPause) { m_Paused = isPause; }
    };
}
