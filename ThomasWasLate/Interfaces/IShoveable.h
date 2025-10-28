#pragma once
#include "Engine/Interfaces/IInterface.h"

namespace thomasWasLate
{
    class IShoveable : public diji::IInterface
    {
    public:
        ~IShoveable() noexcept override = default;
        
        virtual void Shove(const bool isShovingLeft) = 0;
    };
}
