#include "LuckyBlock.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"

void thomasWasLate::LuckyBlock::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();

    m_OriginalPos = m_TransformCompPtr->GetPosition();
}

void thomasWasLate::LuckyBlock::Update()
{
    //temp
    if (!m_IsPlaying) return;

    m_PositionOffset += m_PositionOffsetSpeed * diji::TimeSingleton::GetInstance().GetDeltaTime();
    if (m_PositionOffset <= -20.f && !m_SwitchedDirection)
    {
        m_SwitchedDirection = true;
        m_PositionOffsetSpeed *= -1.f;
    }
    else if (m_PositionOffset >= 0.f)
    {
        m_PositionOffset = 0.f;
        m_IsPlaying = false;
    }
    
    m_TransformCompPtr->SetPosition(m_OriginalPos.x, m_OriginalPos.y + m_PositionOffset);
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
    m_IsPlaying = true;
    
}
