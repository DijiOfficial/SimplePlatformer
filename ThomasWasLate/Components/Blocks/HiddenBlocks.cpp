#include "HiddenBlocks.h"

#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"
#include "../../Helpers/MarioHelpers.h"
#include "../PowerUps/OneUpMushroom.h"

void thomasWasLate::HiddenBlocks::Start()
{
    const auto spriteRender = GetOwner()->GetComponent<diji::SpriteRenderComponent>();
    spriteRender->SetStartingFrame(2, 0);
    spriteRender->SetTotalAnimationFrames(0);
    spriteRender->SetFrameDuration(0);
    spriteRender->SetLooping(false);
    spriteRender->Pause();
    spriteRender->SetCurrentAnimationFrame(0);

    spriteRender->UpdateFrame();
}

void thomasWasLate::HiddenBlocks::OnTriggerEnter(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (collider->GetTag() != "player" || hitInfo.normal.y >= 0.f || m_IsHit)
        return;

    const auto selfCollider = GetOwner()->GetComponent<diji::Collider>();
    const sf::Vector2f playerCenter = collider->GetPosition();
    const sf::FloatRect blockAABB = selfCollider->GetAABB();
    if (mario::MarioHelpers::DoesPlayerHitBottomOfBlock(playerCenter, blockAABB, hitInfo.normal)) return;

    selfCollider->SetCollisionResponse(diji::Collider::CollisionResponse::Block);
    
    mario::MarioHelpers::CheckForCollisionAboveBlock(selfCollider);
    SwitchToEmptyBlockState();
    m_IsHit = true;
}

void thomasWasLate::HiddenBlocks::SpawnStarPowerUp() const
{
    auto oneUpMushroom = std::make_unique<diji::GameObject>();
    oneUpMushroom->AddComponents<diji::Transform>(600, 200);
    oneUpMushroom->AddComponents<diji::TextureComp>("graphics/oneUpMushroom.png");
    oneUpMushroom->AddComponents<diji::Render>();
    oneUpMushroom->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    oneUpMushroom->AddComponents<OneUpMushroom>();

    diji::SceneManager::GetInstance().SpawnGameObject("C_PowerUp", std::move(oneUpMushroom),  GetOwner()->GetComponent<diji::Transform>()->GetPosition());
}

