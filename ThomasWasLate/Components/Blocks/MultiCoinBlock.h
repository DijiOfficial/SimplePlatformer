#pragma once
#include "BreakableBlock.h"
#include "Engine/Singleton/TimerManager.h"

namespace diji
{
    class Timeline;
}

namespace thomasWasLate
{
    class PlayerStates;

    class MultiCoinBlock final : public BreakableBlock
    {
    public:
        explicit MultiCoinBlock(diji::GameObject* ownerPtr) : BreakableBlock{ ownerPtr } {}
        ~MultiCoinBlock() noexcept override = default;

        void Init() override;
        void OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo) override;
        
    private:
        diji::TimerManager::TimerHandle m_TimerHandle{ std::numeric_limits<std::size_t>::max() };
        int m_CoinsLeft = 10;
        bool m_IsLastHit = false;

        void SwitchToEmptyBlockState();
    };
}
