#include "GoombaAI.h"

#include "../Player/PlayerCharacter.h"
#include "../../Singletons/GameManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Singleton/Helpers.h"

void superMarioBros::GoombaAI::HandleStomp(const diji::Collider* other, const std::string& score)
{
    if (other != m_ColliderCompPtr) return;

    m_Paused = true;

    // Change to stomped animation
    const auto m_SpriteRenderComponent = GetOwner()->GetComponent<diji::SpriteRenderComponent>();
    m_SpriteRenderComponent->SetStartingFrame(2, 0);
    m_SpriteRenderComponent->SetTotalAnimationFrames(0);
    m_SpriteRenderComponent->SetFrameDuration(0.1f);
    m_SpriteRenderComponent->SetLooping(false);
    m_SpriteRenderComponent->Pause();
    m_SpriteRenderComponent->SetCurrentAnimationFrame(0);
    m_SpriteRenderComponent->UpdateFrame();

    Destroy(0.65f);

    m_Speed = 0.f;
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ m_Speed, m_ColliderCompPtr->GetVelocity().y });
    SpawnPointsText(score);
}

void superMarioBros::GoombaAI::OnHitEvent(const diji::Collider* other, const diji::CollisionInfo& hitInfo)
{
    if (m_Paused) return;
    if (other->GetTag() == "fireBall")
        return HandleBumpedBehavior(hitInfo.normal.x < 0.f);

    if (diji::Helpers::isZero(hitInfo.normal.x))
        return;

    m_Speed = (hitInfo.normal.x < 0.f) ? -std::abs(m_Speed) : std::abs(m_Speed);
}

void superMarioBros::GoombaAI::HandleBumpedBehavior(const bool isBumpingLeft, const bool addPoints)
{
    if (m_IsDead) return;
    m_IsDead = true;
    
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_kick.wav", false);

    m_TransformCompPtr->SetWorldRotation(sf::degrees(180.f));
    GetOwner()->GetComponent<diji::SpriteRenderComponent>()->Pause();
    
    const sf::Vector2f impulse = isBumpingLeft ? sf::Vector2f{-100.f, -800.f} : sf::Vector2f{100.f, -800.f};
    m_ColliderCompPtr->ApplyImpulse(impulse);
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Ignore);
    m_Paused = true;

    if (!addPoints) return;
    GameManager::SpawnPointsText(m_TransformCompPtr->GetWorldPosition(), "100");
    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(100);
}

void superMarioBros::GoombaAI::Kill(const bool isBumpingLeft, const bool addPoints)
{
    HandleBumpedBehavior(isBumpingLeft, addPoints);
}
