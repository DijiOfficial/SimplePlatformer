#pragma once
#include "Engine/Interfaces/IInterface.h"

namespace thomasWasLate
{
    class IPlayerBumpable : public diji::IInterface
    {
    public:
        ~IPlayerBumpable() noexcept override = default;
        
        virtual void Bump() = 0;
    };
}
