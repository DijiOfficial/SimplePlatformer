#pragma once
#include "../BaseBlock.h"

namespace superMarioBros
{
    class PowerUpBlock final : public BaseBlock
    {
    public:
        using BaseBlock::BaseBlock;
        ~PowerUpBlock() noexcept override = default;

        void Init() override;
        
    private:
        void CreateItemTemplate() override;
        void OnAnimationEnd() override;
    };
}
