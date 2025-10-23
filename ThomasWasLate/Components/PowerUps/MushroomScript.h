#pragma once
#include <string>

#include "../../Interfaces/IBumpable.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class MushroomScript : public IBumpable
    {
    public:
        explicit MushroomScript(diji::GameObject* ownerPtr) : IBumpable{ ownerPtr } {}
        ~MushroomScript() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override; 
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo& hitInfo) override;
        void OnHitEvent(const diji::Collider*, const diji::CollisionInfo& hitInfo) override;

        void HandleBumpedBehavior(bool IsBumpingLeft) override;

    protected:
        std::string m_PointString = "1000";
        diji::Collider* m_ColliderCompPtr = nullptr;
        
    private:
        diji::Transform* m_TransformCompPtr = nullptr;
        bool m_Paused = true;

        float m_Speed = 400.f;

        void PlayStartAnimation();
    };
}
