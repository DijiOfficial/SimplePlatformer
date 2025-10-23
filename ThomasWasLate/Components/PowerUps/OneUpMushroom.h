#pragma once
#include "MushroomScript.h"

namespace diji
{
    class Transform;
}

namespace thomasWasLate
{
    class OneUpMushroom final : public MushroomScript
    {
    public:
        explicit OneUpMushroom(diji::GameObject* ownerPtr) : MushroomScript{ ownerPtr } {}
        ~OneUpMushroom() noexcept override = default;

        void OnEnable() override {}
        void Start() override;

        void OnHitEvent(const diji::Collider*, const diji::CollisionInfo&) override;
    };
}
