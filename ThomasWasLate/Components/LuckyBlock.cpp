#include "LuckyBlock.h"

#include "SmallCoinScript.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/SceneManager.h"

void thomasWasLate::LuckyBlock::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (collider->GetTag() != "player" || hitInfo.normal.y >= 0.f || m_IsHit)
        return;
    m_IsHit = true;

    PlayAnimation();

    // check above to kill enemy
}

void thomasWasLate::LuckyBlock::PlayAnimation() const
{
    const auto spriteRenderComp = GetOwner()->GetComponent<diji::SpriteRenderComponent>();
    
    spriteRenderComp->SetStartingFrame(3, 0);
    spriteRenderComp->SetTotalAnimationFrames(0);
    spriteRenderComp->SetFrameDuration(0);
    spriteRenderComp->SetLooping(false);
    spriteRenderComp->Pause();
    spriteRenderComp->SetCurrentAnimationFrame(0);

    spriteRenderComp->UpdateFrame();

    CreateTimeline();

    auto coinTest = std::make_unique<diji::GameObject>();
    coinTest->AddComponents<diji::Transform>(600, 0);
    coinTest->AddComponents<diji::SpriteRenderComponent>("graphics/smallCoins.png", sf::Vector2i{ 25,50 }, 4, 0.03f);
    coinTest->AddComponents<SmallCoinScript>();
    diji::SceneManager::GetInstance().SpawnGameObject("G_SmallCoin", std::move(coinTest), { GetOwner()->GetComponent<diji::Transform>()->GetPosition().x, GetOwner()->GetComponent<diji::Transform>()->GetPosition().y - 50.f });
}

void thomasWasLate::LuckyBlock::CreateTimeline() const
{
    diji::Timeline* timelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = timelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.1f, .value= -20.f }, { .time= 0.2f, .value= 0.f } };
    
    diji::Transform* transformPtr = GetOwner()->GetComponent<diji::Transform>();
    sf::Vector2f originalPos = transformPtr->GetPosition();
    
    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetPosition(originalPos.x, originalPos.y + y);
    };
}
