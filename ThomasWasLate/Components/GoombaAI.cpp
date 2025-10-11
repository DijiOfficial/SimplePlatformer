#include "GoombaAI.h"

#include "PlayerCharacter.h"
#include "../Singletons/GameManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"
#include "Engine/Singleton/TimeSingleton.h"

void thomasWasLate::GoombaAI::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();

    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnHitByEnemyEvent.AddListener([this]()
    {
        m_Paused = true;
    });

    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnEnemyStompedEvent.AddListener(this, &GoombaAI::HandleStomp);
}

void thomasWasLate::GoombaAI::Update()
{
    if (m_Paused) return;
    
    m_TransformCompPtr->AddOffset(-1 * m_Speed * diji::TimeSingleton::GetInstance().GetDeltaTime(), 0.f);

    if (m_TransformCompPtr->GetPosition().y > 600.f)
        diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
}

void thomasWasLate::GoombaAI::HandleStomp(const diji::Collider* other, const int multiplier)
{
    const auto& collider = GetOwner()->GetComponent<diji::Collider>();
    if (other != collider) return;

    collider->SetAffectedByGravity(false);
    collider->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
    
    m_StompMultiplier = multiplier;
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
    const int score = 100 * m_StompMultiplier;
    const auto& pos = m_TransformCompPtr->GetPosition();
    const auto& yOffset = collider->GetShape()->GetAABB().getSize().y * 3.f;
    const auto& scorePos = sf::Vector2f{ pos.x, pos.y - yOffset };
    GameManager::SpawnPointsText(scorePos, score);
}

void thomasWasLate::GoombaAI::OnHitEvent(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (m_Paused) return;
    if (other->GetTag() == "ground") return;

    m_Speed = -m_Speed;
}

