#include "LuckyBlock.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/SceneManager.h"

void thomasWasLate::LuckyBlock::Init()
{
    // m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
    // m_OriginalPos = m_TransformCompPtr->GetPosition();

    // CreateTimeline();
}

void thomasWasLate::LuckyBlock::Update()
{
    // //temp
    // if (!m_IsPlaying) return;
    //
    // m_PositionOffset += m_PositionOffsetSpeed * diji::TimeSingleton::GetInstance().GetDeltaTime();
    // if (m_PositionOffset <= -20.f && !m_SwitchedDirection)
    // {
    //     m_SwitchedDirection = true;
    //     m_PositionOffsetSpeed *= -1.f;
    // }
    // else if (m_PositionOffset >= 0.f)
    // {
    //     m_PositionOffset = 0.f;
    //     m_IsPlaying = false;
    // }
    //
    // m_TransformCompPtr->SetPosition(m_OriginalPos.x, m_OriginalPos.y + m_PositionOffset);
}

void thomasWasLate::LuckyBlock::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (collider->GetTag() != "player" || hitInfo.normal.y >= 0.f || m_IsHit)
        return;
    m_IsHit = true;

    PlayAnimation();


    // check above to kill enemy
}

void thomasWasLate::LuckyBlock::PlayAnimation()
{
    const auto spriteRenderComp = GetOwner()->GetComponent<diji::SpriteRenderComponent>();
    
    spriteRenderComp->SetStartingFrame(3, 0);
    spriteRenderComp->SetTotalAnimationFrames(0);
    spriteRenderComp->SetFrameDuration(0);
    spriteRenderComp->SetLooping(false);
    spriteRenderComp->Pause();
    spriteRenderComp->SetCurrentAnimationFrame(0);

    spriteRenderComp->UpdateFrame();


    //temp
    // m_IsPlaying = true;
    // m_TimelinePtr->Play(true);
    CreateTimeline();
    
}

void thomasWasLate::LuckyBlock::CreateTimeline() const
{
    diji::Timeline* timelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    // timelinePtr-
    auto &track = timelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.1f, .value= -20.f }, { .time= 0.2f, .value= 0.f } };
    
    diji::Transform* transformPtr = GetOwner()->GetComponent<diji::Transform>();
    sf::Vector2f originalPos = transformPtr->GetPosition();
    
    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetPosition(originalPos.x, originalPos.y + y);
    };
}
