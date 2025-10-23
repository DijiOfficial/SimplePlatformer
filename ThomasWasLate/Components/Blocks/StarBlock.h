#pragma once
#include "BreakableBlock.h"

namespace diji
{
    class Timeline;
}

namespace thomasWasLate
{
    class StarBlock : public BreakableBlock
    {
    public:
        explicit StarBlock(diji::GameObject* ownerPtr) : BreakableBlock{ ownerPtr } {}
        ~StarBlock() noexcept override = default;

        void Init() override;
        void OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo) override;
        
    protected:
        void SwitchToEmptyBlockState() const;
        virtual void SpawnStarPowerUp() const;
    };
}
