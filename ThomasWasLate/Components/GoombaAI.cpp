#include "GoombaAI.h"

#include "PlayerCharacter.h"
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
}

void thomasWasLate::GoombaAI::Update()
{
    if (m_Paused) return;
    
    m_TransformCompPtr->AddOffset(-1 * m_Speed * diji::TimeSingleton::GetInstance().GetDeltaTime(), 0.f);

    if (m_TransformCompPtr->GetPosition().y > 600.f)
        diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
}

void thomasWasLate::GoombaAI::OnHitEvent(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (m_Paused) return;

    if (other->GetTag() == "ground") return;

    m_Speed = -m_Speed;

    if (other->GetTag() != "player") return;

    // mario's hit event is being processed before goomba's. So if goomba gets stomped instead of killing mario we need to add the slop check
    const auto m_SpriteRenderComponent = GetOwner()->GetComponent<diji::SpriteRenderComponent>();

    m_SpriteRenderComponent->SetStartingFrame(2, 0);
    m_SpriteRenderComponent->SetTotalAnimationFrames(0);
    m_SpriteRenderComponent->SetFrameDuration(0.1f);
    m_SpriteRenderComponent->SetLooping(false);
    m_SpriteRenderComponent->Pause();
    m_SpriteRenderComponent->SetCurrentAnimationFrame(0);

    m_SpriteRenderComponent->UpdateFrame();

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
    }, 0.65f, false);

    m_Speed = 0.f;
    m_Paused = true;
}

