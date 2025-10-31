#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class BroadcastPlayerPosition final : public diji::Component
    {
    public:
        explicit BroadcastPlayerPosition(diji::GameObject* ownerPtr) : Component(ownerPtr) {}
        ~BroadcastPlayerPosition() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {} 
        void LateUpdate() override;

        void OnDisable() override {}
        void OnDestroy() override {}

        diji::Event<int> OnPositionMileStoneReachedEvent;

    private:
        diji::Transform* m_TransformCompPtr = nullptr;

        const int BLOCK_SIZE = 50;
        int m_LastBroadcastedMileStone = 0;
        bool m_IsFirstUpdate = true;
    };
}
