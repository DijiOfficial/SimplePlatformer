#pragma once
#include "BasePowerUp.h"
#include "../../Interfaces/IBumpable.h"

namespace thomasWasLate
{
    class MushroomScript final : public BasePowerUp, public IBumpable
    {
    public:
        using BasePowerUp::BasePowerUp;
        ~MushroomScript() noexcept override = default;

        void HandleBumpedBehavior(bool IsBumpingLeft, const bool addPoints = true) override;
        
    private:
        void OnAnimationComplete() override;
        bool m_HasBeenBumped = false;
    };
}
