#include "MushroomScript.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Singleton/TimerManager.h"

void superMarioBros::MushroomScript::HandleBumpedBehavior(const bool IsBumpingLeft, const bool)
{
    if (m_HasBeenBumped) return;
    m_HasBeenBumped = true;

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        m_HasBeenBumped = false;
    }, 0.067f, false);
    
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0, 0 });
    const sf::Vector2f impulse = IsBumpingLeft ? sf::Vector2f{-200.f, -1000.f} : sf::Vector2f{200.f, -1000.f};
    m_ColliderCompPtr->ApplyImpulse(impulse);
    m_Speed = IsBumpingLeft ? -std::abs(m_Speed) : std::abs(m_Speed);
}

void superMarioBros::MushroomScript::OnAnimationComplete()
{
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Block);
    m_ColliderCompPtr->SetGenerateHitEvents(true);
    m_ColliderCompPtr->SetRestitution(0.f);
    m_ColliderCompPtr->SetMass(0.89f);
    m_ColliderCompPtr->SetStaticFriction(0.25f);
    m_ColliderCompPtr->SetAffectedByGravity(true);
    m_ColliderCompPtr->SetTag("powerUp");
    m_Paused = false;
}