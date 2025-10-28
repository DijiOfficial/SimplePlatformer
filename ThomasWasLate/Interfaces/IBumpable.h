#pragma once
#include "Engine/Components/Component.h"

namespace thomasWasLate
{
    class IBumpable : public diji::Component
    {
    public:
        explicit IBumpable(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~IBumpable() noexcept override = default;

        void Init() override = 0;
        void OnEnable() override = 0;
        void Start() override = 0;
        
        void Update() override = 0;
        void FixedUpdate() override = 0;
        void LateUpdate() override = 0;

        void OnDisable() override = 0;
        void OnDestroy() override = 0;

        virtual void HandleBumpedBehavior(const bool IsBumpingLeft, const bool addPoints = true) = 0; 
    };
}