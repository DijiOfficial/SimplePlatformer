#pragma once
#include "Engine/Interfaces/IInterface.h"

namespace superMarioBros
{
    class IBumpable : public diji::IInterface
    {
    public:
        ~IBumpable() noexcept override = default;
        
        virtual void HandleBumpedBehavior(const bool IsBumpingLeft, const bool addPoints = true) = 0; 
    };
}
