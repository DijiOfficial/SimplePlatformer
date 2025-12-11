#pragma once
#include "BasePowerUp.h"

namespace superMarioBros
{
    class StarPower final : public BasePowerUp
    {
    public:
        using BasePowerUp::BasePowerUp;
        ~StarPower() noexcept override = default;

        void Init() override;
        void OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo& hitInfo) override;

    protected:
        void OnAnimationComplete() override;
    };
}
