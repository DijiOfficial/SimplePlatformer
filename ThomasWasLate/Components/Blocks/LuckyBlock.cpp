#include "LuckyBlock.h"

#include "../PowerUps/FireFlower.h"
#include "../PowerUps/MushroomScript.h"
#include "../../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/SceneManager.h"
#include "../../Helpers/MarioHelpers.h"

void thomasWasLate::LuckyBlock::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (collider->GetTag() != "player" || hitInfo.normal.y >= 0.f || m_IsHit)
        return;

    const auto selfCollider = GetOwner()->GetComponent<diji::Collider>();
    const sf::Vector2f playerCenter = collider->GetPosition();
    const sf::FloatRect blockAABB = selfCollider->GetAABB();
    if (mario::MarioHelpers::DoesPlayerHitBottomOfBlock(playerCenter, blockAABB, hitInfo.normal)) return;

    m_IsHit = true;
    PlayAnimation();

    mario::MarioHelpers::CheckForCollisionAboveBlock(selfCollider);
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

    if (m_IsPowerUpBlock) return;
    
    mario::MarioHelpers::SpawnCoinAboveBlock(GetOwner()->GetComponent<diji::Transform>()->GetPosition());
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

    if (!m_IsPowerUpBlock) return;
    
    auto& [eventName, eventKeysVec] = timelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.2f, .callback= [&]()
            {
                SpawnPowerUp();
            }
        }
    };
}

void thomasWasLate::LuckyBlock::SpawnPowerUp() const
{
    auto powerUp = std::make_unique<diji::GameObject>();
    powerUp->AddComponents<diji::Transform>(1000, 300);
    powerUp->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    powerUp->GetComponent<diji::Collider>()->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
    powerUp->GetComponent<diji::Collider>()->SetAffectedByGravity(false);
    powerUp->GetComponent<diji::Collider>()->SetTag("powerUp");
    
    if (GameManager::GetInstance().GetCurrentPlayerState() == PlayerHealthState::Small)
    {
        powerUp->AddComponents<diji::TextureComp>("graphics/mushroom.png");
        powerUp->AddComponents<diji::Render>();
        powerUp->AddComponents<MushroomScript>();
    }
    else
    {
        powerUp->AddComponents<diji::SpriteRenderComponent>("graphics/fireFlower.png", sf::Vector2i{ 50, 50 }, 4, 0.065f);
        powerUp->AddComponents<FireFlower>();
    }


    diji::SceneManager::GetInstance().SpawnGameObject("C_PowerUp", std::move(powerUp), GetOwner()->GetComponent<diji::Transform>()->GetPosition());
}
