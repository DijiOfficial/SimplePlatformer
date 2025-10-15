#include "LuckyBlock.h"

#include "MushroomScript.h"
#include "SmallCoinScript.h"
#include "../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"

void thomasWasLate::LuckyBlock::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& hitInfo)
{
    if (collider->GetTag() != "player" || hitInfo.normal.y >= 0.f || m_IsHit)
        return;

    const sf::Vector2f playerCenter = collider->GetPosition();
    const sf::FloatRect blockAABB = GetOwner()->GetComponent<diji::Collider>()->GetAABB();

    const sf::Vector2f blockCenter{ blockAABB.left + blockAABB.width * 0.5f, blockAABB.top  + blockAABB.height * 0.5f };
    const sf::Vector2f halfExtents{ blockAABB.width * 0.5f, blockAABB.height * 0.5f };

    const sf::Vector2f normal = diji::Helpers::Normalize(hitInfo.normal);
    const sf::Vector2f tangent{ -normal.y, normal.x };

    const sf::Vector2f rel = playerCenter - blockCenter;
    const float coordAlongTangent = rel.x * tangent.x + rel.y * tangent.y;
    const float halfExtentAlongTangent = std::abs(tangent.x) * halfExtents.x + std::abs(tangent.y) * halfExtents.y + 8.f;

    // is block above player center
    if (std::abs(coordAlongTangent) > halfExtentAlongTangent)
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

    if (m_IsPowerUpBlock) return;
    
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

    if (m_IsPowerUpBlock)
    {
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
}

void thomasWasLate::LuckyBlock::SpawnPowerUp() const
{
    auto powerUp = std::make_unique<diji::GameObject>();
    if (GameManager::GetInstance().GetCurrentPlayerState() == PlayerHealthState::Small)
    {
        powerUp->AddComponents<diji::Transform>(1000, 300);
        powerUp->AddComponents<diji::TextureComp>("graphics/mushroom.png");
        powerUp->AddComponents<diji::Render>();
        powerUp->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
        powerUp->GetComponent<diji::Collider>()->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
        powerUp->GetComponent<diji::Collider>()->SetAffectedByGravity(false);
        powerUp->AddComponents<MushroomScript>();
    }
    else
    {
        // Handle other power-ups here 
    }

    diji::SceneManager::GetInstance().SpawnGameObject("C_PowerUp", std::move(powerUp), GetOwner()->GetComponent<diji::Transform>()->GetPosition());
}
