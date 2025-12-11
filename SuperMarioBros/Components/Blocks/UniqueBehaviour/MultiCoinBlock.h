#pragma once
#include "../BaseBlock.h"
#include "Engine/Singleton/TimerManager.h"

namespace superMarioBros
{
    class MultiCoinBlock final : public BaseBlock
    {
    public:
        using BaseBlock::BaseBlock;
        ~MultiCoinBlock() noexcept override = default;

        void Init() override;
        
    private:
        diji::TimerManager::TimerHandle m_TimerHandle{ std::numeric_limits<std::size_t>::max() };
        int m_CoinsLeft = 10;
        bool m_IsLastHit = false;
        bool m_CanBeHit = true;

        void OnAnimationStart() override;
        void OnAnimationEnd() override;
    };
}
