#pragma once
#include "Engine/Interfaces/IInterface.h"

namespace superMarioBros
{
    class IShoveable : public diji::IInterface
    {
    public:
        ~IShoveable() noexcept override = default;
        
        virtual void Shove(const bool isShovingLeft) = 0;
    };
}
