#include "GoombaAI.h"

#include "PlayerCharacter.h"
#include "../Singletons/GameManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"
#include "Engine/Singleton/TimeSingleton.h"

void thomasWasLate::GoombaAI::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();

    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnHitByEnemyEvent.AddListener([this]()
    {
        m_Paused = true;
    });

    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnEnemyStompedEvent.AddListener(this, &GoombaAI::HandleStomp);
    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnPoweringUpEvent.AddListener(this, &GoombaAI::SetPauseState);
}

void thomasWasLate::GoombaAI::Update()
{
    if (m_TransformCompPtr->GetPosition().y > 600.f)
        diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
}

void thomasWasLate::GoombaAI::FixedUpdate()
{
    if (m_Paused) return;
    
    m_TransformCompPtr->AddOffset(m_Speed * diji::TimeSingleton::GetInstance().GetFixedUpdateDeltaTime(), 0.f);
}

void thomasWasLate::GoombaAI::HandleStomp(const diji::Collider* other, const std::string& score)
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

    // Set timer to destroy after 0.65 seconds
    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
    }, 0.65f, false);

    // Stop moving
    m_Speed = 0.f;
    
    // Spawn points text
    const auto& pos = m_TransformCompPtr->GetPosition();
    const auto& yOffset = m_ColliderCompPtr->GetShape()->GetAABB().getSize().y * 3.f;
    const auto& scorePos = sf::Vector2f{ pos.x, pos.y - yOffset };
    GameManager::SpawnPointsText(scorePos, score);
}

void thomasWasLate::GoombaAI::OnHitEvent(const diji::Collider* other, const diji::CollisionInfo& hitInfo)
{
    if (m_Paused) return;
    if (other->GetTag() == "fireBall")
        return HandleBumpedBehavior(hitInfo.normal.x < 0.f);

    if (diji::Helpers::isZero(hitInfo.normal.x))
        return;

    m_Speed = -m_Speed;
}

void thomasWasLate::GoombaAI::HandleBumpedBehavior(const bool IsBumpingLeft)
{
    m_TransformCompPtr->SetRotation(180.f);
    GetOwner()->GetComponent<diji::SpriteRenderComponent>()->Pause();
    
    const sf::Vector2f impulse = IsBumpingLeft ? sf::Vector2f{-300.f, -1200.f} : sf::Vector2f{300.f, -1200.f};
    m_ColliderCompPtr->ApplyImpulse(impulse);
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
    m_Paused = true;
    
    GameManager::SpawnPointsText(m_TransformCompPtr->GetPosition(), "100");
    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(100);
}

