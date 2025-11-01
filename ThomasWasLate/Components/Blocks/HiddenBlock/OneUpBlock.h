#pragma once
#include "HiddenBlock.h"

namespace thomasWasLate
{
    class OneUpBlock final : public HiddenBlock
    {
    public:
        using HiddenBlock::HiddenBlock;
        ~OneUpBlock() noexcept override = default;

        void Init() override;
        
    private:
        void CreateItemTemplate() override;
    };
}
