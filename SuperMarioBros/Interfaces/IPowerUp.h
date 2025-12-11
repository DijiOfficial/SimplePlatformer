#pragma once
#include "Engine/Interfaces/IInterface.h"

namespace superMarioBros
{
    class IPowerUp : public diji::IInterface
    {
    public:
        ~IPowerUp() noexcept override = default;

        enum class PowerUpType : uint8_t
        {
            None = 255,
            Mushroom = 0,
            OneUpMushroom = 1,
            FireFlower = 2,
            Star = 3,
        };
        
        virtual void OnPowerUpCollected(PowerUpType power) = 0;
    };
}
