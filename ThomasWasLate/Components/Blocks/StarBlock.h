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
        void SwitchToEmptyBlockState() const;
        void SpawnStarPowerUp() const;
    };
}
