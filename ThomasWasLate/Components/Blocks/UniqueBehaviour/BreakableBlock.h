#pragma once
#include "../BaseBlock.h"

namespace thomasWasLate
{
    class BreakableBlock final : public BaseBlock
    {
    public:
        using BaseBlock::BaseBlock;
        ~BreakableBlock() noexcept override = default;

        void Init() override;

    private:
        void OnAnimationStart() override;
    };
}
