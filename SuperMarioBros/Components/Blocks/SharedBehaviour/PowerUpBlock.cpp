#include "PowerUpBlock.h"
#include "../../PowerUps/MushroomScript.h"
#include "../../../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/GameObject.h"

void superMarioBros::PowerUpBlock::Init()
{
    BaseBlock::Init();
    
    m_AudioName = "smb_powerup_appears.wav";
}

void superMarioBros::PowerUpBlock::CreateItemTemplate()
{
    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_ItemTemplateUPtr->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    m_ItemTemplateUPtr->GetComponent<diji::Collider>()->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
    m_ItemTemplateUPtr->GetComponent<diji::Collider>()->SetAffectedByGravity(false);
    m_ItemTemplateUPtr->GetComponent<diji::Collider>()->SetTag("powerUp");
}

void superMarioBros::PowerUpBlock::OnAnimationEnd()
{
    if (GameManager::GetInstance().GetCurrentPlayerState() == PlayerHealthState::Small)
    {
        m_ItemTemplateUPtr->AddComponent<diji::TextureComp>("graphics/mushroom.png");
        m_ItemTemplateUPtr->AddComponent<diji::Render>();
        m_ItemTemplateUPtr->AddComponent<MushroomScript>(IPowerUp::PowerUpType::Mushroom, true);
    }
    else
    {
        m_ItemTemplateUPtr->AddComponent<diji::SpriteRenderComponent>("graphics/fireFlower.png", sf::Vector2i{ 50, 50 }, 4, 0.065f);
        m_ItemTemplateUPtr->AddComponent<BasePowerUp>(IPowerUp::PowerUpType::FireFlower, false);
    }
}
