#include "StarPower.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Singleton/Helpers.h"

void thomasWasLate::StarPower::Init()
{
    BasePowerUp::Init();
    
    m_ColliderCompPtr->SetRestitution(1.5f);
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
    m_ColliderCompPtr->SetStaticFriction(0.f);
    m_ColliderCompPtr->SetMaxVelocity(sf::Vector2f{ 600.f, 800.f });
    m_ColliderCompPtr->SetAffectedByGravity(false);
}

void thomasWasLate::StarPower::OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo& hitInfo)
{
    if (other->GetTag() == "ground")
    {
        const auto& velocity = m_ColliderCompPtr->GetVelocity() * 1.2f;
        if (diji::Helpers::isZero(hitInfo.normal.y))
            m_ColliderCompPtr->SetVelocity(sf::Vector2f{ velocity.x * hitInfo.normal.x, velocity.y });
        else
            m_ColliderCompPtr->SetVelocity(sf::Vector2f{ velocity.x, velocity.y * hitInfo.normal.y });
    }

    BasePowerUp::OnTriggerEnter(other, hitInfo);
}

void thomasWasLate::StarPower::OnAnimationComplete()
{
    m_ColliderCompPtr->SetAffectedByGravity(true);
    m_ColliderCompPtr->ApplyImpulse(sf::Vector2f{ 300.f, -800.f });
}