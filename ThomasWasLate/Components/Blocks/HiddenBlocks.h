#pragma once
#include "StarBlock.h"

namespace diji
{
    class Timeline;
}

namespace thomasWasLate
{
    class HiddenBlocks final : public StarBlock
    {
    public:
        explicit HiddenBlocks(diji::GameObject* ownerPtr) : StarBlock{ ownerPtr } {}
        ~HiddenBlocks() noexcept override = default;

        void Start() override;
        void OnTriggerEnter(const diji::Collider* collider, const diji::CollisionInfo& hitInfo) override;
        
    private:
        void SpawnStarPowerUp() const override;
    };
}
