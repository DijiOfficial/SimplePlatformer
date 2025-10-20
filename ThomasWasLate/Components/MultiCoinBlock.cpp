#include "MultiCoinBlock.h"
#include "Engine/Collision/Collider.h"
#include "../Helpers/MarioHelpers.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Interfaces/Timeline.h"

void thomasWasLate::MultiCoinBlock::Init()
{
    BreakableBlock::Init();

    auto& [eventName, eventKeysVec] = m_TimelinePtr->GetEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.2f, .callback= [&]()
            {
                m_TimerHandle = diji::TimerManager::GetInstance().SetTimer([&]()
                {
                    m_IsLastHit = true;
                }, 1.f, false);
            }
        }
    };
}

void thomasWasLate::MultiCoinBlock::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (collider->GetTag() != "player" || hitInfo.normal.y >= 0.f || m_IsHit)
        return;

    const auto selfCollider = GetOwner()->GetComponent<diji::Collider>();
    const sf::Vector2f playerCenter = collider->GetPosition();
    const sf::FloatRect blockAABB = selfCollider->GetAABB();
    if (mario::MarioHelpers::DoesPlayerHitBottomOfBlock(playerCenter, blockAABB, hitInfo.normal)) return;

    mario::MarioHelpers::CheckForCollisionAboveBlock(selfCollider);

    m_TimelinePtr->PlayFromStart();

    mario::MarioHelpers::SpawnCoinAboveBlock(GetOwner()->GetComponent<diji::Transform>()->GetPosition());

    diji::TimerManager::GetInstance().ClearTimer(m_TimerHandle);

    --m_CoinsLeft;
    if (m_CoinsLeft <= 0 || m_IsLastHit)
        SwitchToEmptyBlockState();
}

void thomasWasLate::MultiCoinBlock::SwitchToEmptyBlockState()
{
    m_IsHit = true;
    
    const auto spriteRenderComp = GetOwner()->GetComponent<diji::SpriteRenderComponent>();
        
    spriteRenderComp->SetStartingFrame(1, 0);
    spriteRenderComp->SetTotalAnimationFrames(0);
    spriteRenderComp->SetFrameDuration(0);
    spriteRenderComp->SetLooping(false);
    spriteRenderComp->Pause();
    spriteRenderComp->SetCurrentAnimationFrame(0);

    spriteRenderComp->UpdateFrame();
}
