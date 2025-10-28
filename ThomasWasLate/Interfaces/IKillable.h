#pragma once
#include "Engine/Interfaces/IInterface.h"

namespace thomasWasLate
{
    class IKillable : public diji::IInterface
    {
    public:
        ~IKillable() noexcept override = default;
        
        virtual void Kill(const bool isBumpingLeft, const bool addPoints = true) = 0;
    };
}
