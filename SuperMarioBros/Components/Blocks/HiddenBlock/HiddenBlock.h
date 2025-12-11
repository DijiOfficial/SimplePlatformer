#pragma once
#include "../BaseBlock.h"

namespace superMarioBros
{
    class HiddenBlock : public BaseBlock
    {
    public:
        using BaseBlock::BaseBlock;
        ~HiddenBlock() noexcept override = default;

        void Start() override;
        
    private:
        void OnAnimationStart() override;
    };
}
