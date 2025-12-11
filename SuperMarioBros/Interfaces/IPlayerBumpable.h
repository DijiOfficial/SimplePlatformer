#pragma once
#include "Engine/Interfaces/IInterface.h"

namespace superMarioBros
{
    class IPlayerBumpable : public diji::IInterface
    {
    public:
        ~IPlayerBumpable() noexcept override = default;
        
        virtual void Bump() = 0;
    };
}
