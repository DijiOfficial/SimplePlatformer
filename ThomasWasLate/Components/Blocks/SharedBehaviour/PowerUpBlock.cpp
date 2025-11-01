#include "PowerUpBlock.h"
#include "../../PowerUps/MushroomScript.h"
#include "../../../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"

void thomasWasLate::PowerUpBlock::Init()
{
    BaseBlock::Init();
    
    m_AudioName = "smb_powerup_appears.wav";
}

void thomasWasLate::PowerUpBlock::CreateItemTemplate()
{
    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_ItemTemplateUPtr->AddComponents<diji::Transform>(0, 0);
    m_ItemTemplateUPtr->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    m_ItemTemplateUPtr->GetComponent<diji::Collider>()->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
    m_ItemTemplateUPtr->GetComponent<diji::Collider>()->SetAffectedByGravity(false);
    m_ItemTemplateUPtr->GetComponent<diji::Collider>()->SetTag("powerUp");
}

void thomasWasLate::PowerUpBlock::OnAnimationEnd()
{
    if (GameManager::GetInstance().GetCurrentPlayerState() == PlayerHealthState::Small)
    {
        m_ItemTemplateUPtr->AddComponents<diji::TextureComp>("graphics/mushroom.png");
        m_ItemTemplateUPtr->AddComponents<diji::Render>();
        m_ItemTemplateUPtr->AddComponents<MushroomScript>(IPowerUp::PowerUpType::Mushroom, true);
    }
    else
    {
        m_ItemTemplateUPtr->AddComponents<diji::SpriteRenderComponent>("graphics/fireFlower.png", sf::Vector2i{ 50, 50 }, 4, 0.065f);
        m_ItemTemplateUPtr->AddComponents<BasePowerUp>(IPowerUp::PowerUpType::FireFlower, false);
    }
}
