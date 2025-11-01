#pragma once
#include "../BaseBlock.h"

namespace thomasWasLate
{
    class StarBlock final : public BaseBlock
    {
    public:
        using BaseBlock::BaseBlock;
        ~StarBlock() noexcept override = default;

        void Init() override;
        
    private:
        void CreateItemTemplate() override;
    };
}
