#include "StarBlock.h"

#include "Engine/Collision/Collider.h"
#include "../../Helpers/MarioHelpers.h"
#include "../PowerUps/StartPower.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
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
    star->GetComponent<diji::Collider>()->SetRestitution(1.5f);
    star->GetComponent<diji::Collider>()->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
    star->GetComponent<diji::Collider>()->SetStaticFriction(0.f);
    star->GetComponent<diji::Collider>()->SetMaxVelocity(sf::Vector2f{ 600.f, 1000.f });
    star->GetComponent<diji::Collider>()->SetTag("star");
    star->GetComponent<diji::Collider>()->SetAffectedByGravity(false);
    star->AddComponents<StartPower>();

    diji::SceneManager::GetInstance().SpawnGameObject("C_PowerUp", std::move(star),  GetOwner()->GetComponent<diji::Transform>()->GetPosition());
}
