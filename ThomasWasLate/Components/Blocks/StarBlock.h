#pragma once
#include "BreakableBlock.h"
#include "Engine/Singleton/TimerManager.h"

namespace diji
{
    class Timeline;
}

namespace thomasWasLate
{
    class StarBlock final : public BreakableBlock
    {
    public:
        explicit StarBlock(diji::GameObject* ownerPtr) : BreakableBlock{ ownerPtr } {}
        ~StarBlock() noexcept override = default;

        void Init() override;
        void OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo) override;
        
    private:
        diji::TimerManager::TimerHandle m_TimerHandle{ std::numeric_limits<std::size_t>::max() };
        int m_CoinsLeft = 10;
        bool m_IsLastHit = false;

        void SwitchToEmptyBlockState() const;
        void SpawnStarPowerUp() const;
    };
}
