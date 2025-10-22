#pragma once
#include "Engine/Interfaces/IInterface.h"

namespace diji
{
    class IKillable : public IInterface
    {
    public:
        ~IKillable() noexcept override = default;
        
        virtual void Kill(const bool isBumpingLeft) = 0;
    };
}
