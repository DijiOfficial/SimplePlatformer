#include "StarBlock.h"

#include "Engine/Collision/Collider.h"
#include "../../Helpers/MarioHelpers.h"
#include "../PowerUps/StarPower.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/SceneManager.h"

void thomasWasLate::StarBlock::Init()
{
    BreakableBlock::Init();

    auto& [eventName, eventKeysVec] = m_TimelinePtr->GetEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.2f, .callback= [&]()
            {
                SpawnStarPowerUp();
            }
        }
    };
}

void thomasWasLate::StarBlock::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (collider->GetTag() != "player" || hitInfo.normal.y >= 0.f || m_IsHit)
        return;
    
    const auto selfCollider = GetOwner()->GetComponent<diji::Collider>();
    const sf::Vector2f playerCenter = collider->GetPosition();
    const sf::FloatRect blockAABB = selfCollider->GetAABB();
    if (mario::MarioHelpers::DoesPlayerHitBottomOfBlock(playerCenter, blockAABB, hitInfo.normal)) return;

    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_powerup_appears.wav", false);
    mario::MarioHelpers::CheckForCollisionAboveBlock(selfCollider);
    SwitchToEmptyBlockState();
    m_IsHit = true;
}

void thomasWasLate::StarBlock::SwitchToEmptyBlockState() const
{
    m_TimelinePtr->PlayFromStart();

    const auto spriteRenderComp = GetOwner()->GetComponent<diji::SpriteRenderComponent>();
        
    spriteRenderComp->SetStartingFrame(1, 0);
    spriteRenderComp->SetTotalAnimationFrames(0);
    spriteRenderComp->SetFrameDuration(0);
    spriteRenderComp->SetLooping(false);
    spriteRenderComp->Pause();
    spriteRenderComp->SetCurrentAnimationFrame(0);

    spriteRenderComp->UpdateFrame();
}

void thomasWasLate::StarBlock::SpawnStarPowerUp() const
{
    auto star = std::make_unique<diji::GameObject>();
    star->AddComponents<diji::Transform>(600, 200);
    star->AddComponents<diji::SpriteRenderComponent>("graphics/star.png", sf::Vector2i{ 50,50 }, 4, 0.035f);
    star->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    star->AddComponents<StarPower>(IPowerUp::Star, false);

    diji::SceneManager::GetInstance().SpawnGameObject("C_PowerUp", std::move(star),  GetOwner()->GetComponent<diji::Transform>()->GetPosition());
}
