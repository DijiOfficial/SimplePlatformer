#include "KoopaTroopa.h"

#include "../../Helpers/MarioHelpers.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "../../Singletons/GameManager.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Singleton/Helpers.h"

void superMarioBros::KoopaTroopa::Start()
{
    BaseEnemy::Start();
    
    const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar");
    m_EnemyColliderCompPtr = player->GetComponent<diji::Collider>();
}

void superMarioBros::KoopaTroopa::HandleStomp(const diji::Collider* other, const std::string& score)
{
    if (other != m_ColliderCompPtr) return;
    switch (m_KoopaTroopaState)
    {
    case KoopaTroopaState::Alive:
        HandleStomp();
        break;
    case KoopaTroopaState::Stomped:
        HandleBumped();
        break;
    case KoopaTroopaState::Bumped:
        HandleStopBumpMovement();
        break;
    }

    m_Paused = false;
    SpawnPointsText(score);
}

void superMarioBros::KoopaTroopa::OnHitEvent(const diji::Collider* other, const diji::CollisionInfo& hitInfo)
{
    if (m_Paused) return;
    if (other->GetTag() == "fireBall")
        return Kill(other->GetPosition().x > m_TransformCompPtr->GetPosition().x);

    if (diji::Helpers::isZero(hitInfo.normal.x))
        return;

    m_Speed = (hitInfo.normal.x < 0.f) ? -std::abs(m_Speed) : std::abs(m_Speed);

    m_SpriteRenderCompPtr->SetScale(std::signbit(m_Speed) ? -1.f : 1.f);
    m_SpriteRenderCompPtr->InvertSprite();
}

void superMarioBros::KoopaTroopa::OnTriggerExit(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (other->GetTag() != "player") return;
    
    if (m_KoopaTroopaState == KoopaTroopaState::Bumped)
        m_ColliderCompPtr->ClearOverlappedCollider(m_EnemyColliderCompPtr);
}

void superMarioBros::KoopaTroopa::OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (other->GetTag() == "fireBall")
        return Kill(other->GetPosition().x < m_TransformCompPtr->GetPosition().x);

    if (m_KoopaTroopaState != KoopaTroopaState::Bumped) return;
    
    if (other->GetTag() == "enemy" || other->GetTag() == "koopa")
    {
        const auto enemyInterface = diji::InterfaceRegistry::GetInterface<IKillable>(other->GetParent());
        enemyInterface->Kill(m_TransformCompPtr->GetPosition().x > other->GetPosition().x, false);

        SpawnPointsText(MarioHelpers::GetStompPointsAsString(m_CurrentComboIndex));
        ++m_CurrentComboIndex;
    }
}

void superMarioBros::KoopaTroopa::HandleBumpedBehavior(const bool, const bool)
{
    // m_TransformCompPtr->SetRotation(180.f);
    // GetOwner()->GetComponent<diji::SpriteRenderComponent>()->Pause();
    //
    // const sf::Vector2f impulse = isBumpingLeft ? sf::Vector2f{-300.f, -1200.f} : sf::Vector2f{300.f, -1200.f};
    // m_ColliderCompPtr->ApplyImpulse(impulse);
    // m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
    // m_Paused = true;
    //
    // GameManager::SpawnPointsText(m_TransformCompPtr->GetPosition(), "100");
    // GameManager::GetInstance().OnScoreAddedEvent.Broadcast(100);
}

void superMarioBros::KoopaTroopa::Kill(const bool isBumpingLeft, const bool)
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_kick.wav", false);
    
    diji::TimerManager::GetInstance().ClearTimer(m_TimerHandle);
    m_TransformCompPtr->SetRotation(sf::degrees(180.f));
    m_SpriteRenderCompPtr->SetStartingFrame(4, 0);
    m_SpriteRenderCompPtr->SetTotalAnimationFrames(0);
    m_SpriteRenderCompPtr->SetFrameDuration(0.1f);
    m_SpriteRenderCompPtr->SetLooping(false);
    m_SpriteRenderCompPtr->Pause();
    m_SpriteRenderCompPtr->SetCurrentAnimationFrame(0);
    m_SpriteRenderCompPtr->UpdateFrame();

    m_ColliderCompPtr->SetIsMoveable(true);
    m_ColliderCompPtr->ClearAllOverlappedCollider();
    const sf::Vector2f impulse = isBumpingLeft ? sf::Vector2f{-300.f, -1200.f} : sf::Vector2f{300.f, -1200.f};
    m_ColliderCompPtr->ApplyImpulse(impulse);
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Ignore);
    m_Paused = true;
    
    GameManager::SpawnPointsText(m_TransformCompPtr->GetPosition(), "200");
    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(200);

    diji::TimerManager::GetInstance().ClearTimer(m_TimerHandle);
}

void superMarioBros::KoopaTroopa::Shove(const bool isShovingLeft)
{
    m_Speed = isShovingLeft ? -m_ShellSpeed : m_ShellSpeed;
    m_Paused = false;
}

void superMarioBros::KoopaTroopa::HandleBumped()
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_kick.wav", false);

    diji::TimerManager::GetInstance().ClearTimer(m_TimerHandle);
    SetShellAppearance();
    
    m_ColliderCompPtr->SetIsMoveable(true);
    m_KoopaTroopaState = KoopaTroopaState::Bumped;

    for (const auto enemyCollider : GameManager::GetInstance().GetEnemyColliders())
    {
        if (enemyCollider == m_ColliderCompPtr) continue;
        m_ColliderCompPtr->OverlapCollider(enemyCollider);
    }
}

void superMarioBros::KoopaTroopa::SetRespawnTimer()
{
    m_TimerHandle = diji::TimerManager::GetInstance().SetTimer([&]()
    {
        m_SpriteRenderCompPtr->SetStartingFrame(4, 0);
        m_SpriteRenderCompPtr->SetTotalAnimationFrames(2);
        m_SpriteRenderCompPtr->SetFrameDuration(0.135f);
        m_SpriteRenderCompPtr->SetLooping(true);
        m_SpriteRenderCompPtr->Play();
        m_SpriteRenderCompPtr->SetCurrentAnimationFrame(0);
        m_SpriteRenderCompPtr->UpdateFrame();

        m_TimerHandle = diji::TimerManager::GetInstance().SetTimer([&]()
        {
            m_ColliderCompPtr->ClearOverlappedCollider(m_EnemyColliderCompPtr);
            m_ColliderCompPtr->SetIsMoveable(true);
            m_KoopaTroopaState = KoopaTroopaState::Alive;
                
            m_SpriteRenderCompPtr->SetStartingFrame(0, 0);
            m_SpriteRenderCompPtr->SetTotalAnimationFrames(2);
            m_SpriteRenderCompPtr->SetFrameDuration(0.15f);
            m_SpriteRenderCompPtr->SetLooping(true);
            m_SpriteRenderCompPtr->Play();
            m_SpriteRenderCompPtr->SetCurrentAnimationFrame(0);
            m_SpriteRenderCompPtr->UpdateFrame();

            // can load direction here
            m_Speed = BASE_SPEED;
            m_SpriteRenderCompPtr->SetScale(std::signbit(m_Speed) ? -1.f : 1.f);
            m_SpriteRenderCompPtr->InvertSprite();
            m_Paused = false;
        }, 1.5f, false);
        
    }, 4.f, false);
}

void superMarioBros::KoopaTroopa::SetShellAppearance() const
{
    m_SpriteRenderCompPtr->SetStartingFrame(4, 0);
    m_SpriteRenderCompPtr->SetTotalAnimationFrames(0);
    m_SpriteRenderCompPtr->SetFrameDuration(0.1f);
    m_SpriteRenderCompPtr->SetLooping(false);
    m_SpriteRenderCompPtr->Pause();
    m_SpriteRenderCompPtr->SetCurrentAnimationFrame(0);
    m_SpriteRenderCompPtr->UpdateFrame();
}

void superMarioBros::KoopaTroopa::HandleStomp()
{
    SetRespawnTimer();
    
    m_CurrentComboIndex = BASE_COMBO_INDEX;
    m_ColliderCompPtr->OverlapCollider(m_EnemyColliderCompPtr);
    m_ColliderCompPtr->SetIsMoveable(false);
    m_KoopaTroopaState = KoopaTroopaState::Stomped;
    
    SetShellAppearance();

    m_Speed = 0.f;
}

void superMarioBros::KoopaTroopa::HandleStopBumpMovement()
{
    SetRespawnTimer();
    m_ColliderCompPtr->ClearAllOverlappedCollider();
    m_CurrentComboIndex = BASE_COMBO_INDEX;
    
    m_ColliderCompPtr->OverlapCollider(m_EnemyColliderCompPtr);
    m_ColliderCompPtr->SetIsMoveable(false);
    m_KoopaTroopaState = KoopaTroopaState::Stomped;
    m_Speed = 0.f;
}
