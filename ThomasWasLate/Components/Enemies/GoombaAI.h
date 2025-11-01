#pragma once
#include <SFML/Graphics/Glsl.hpp>

#include "BaseEnemy.h"

namespace thomasWasLate
{
    class GoombaAI final : public BaseEnemy
    {
    public:
        explicit GoombaAI(diji::GameObject* ownerPtr) : BaseEnemy{ ownerPtr } {}
        ~GoombaAI() noexcept override = default;

        void HandleStomp(const diji::Collider* other, const std::string& score) override;
        void OnHitEvent(const diji::Collider* other, const diji::CollisionInfo& hitInfo) override;

        void HandleBumpedBehavior(bool isBumpingLeft, const bool addPoints = true) override;

        void Kill(bool isBumpingLeft, const bool addPoints = true) override;

    private:
        bool m_HasBeenBumped = false;
    };
}
