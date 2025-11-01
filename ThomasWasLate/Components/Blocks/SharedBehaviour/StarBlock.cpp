#include "StarBlock.h"
#include "Engine/Collision/Collider.h"
#include "../../PowerUps/StarPower.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"

void thomasWasLate::StarBlock::Init()
{
    BaseBlock::Init();

    m_AudioName = "smb_powerup_appears.wav";
}

void thomasWasLate::StarBlock::CreateItemTemplate()
{
    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_ItemTemplateUPtr->AddComponents<diji::Transform>(0, 0);
    m_ItemTemplateUPtr->AddComponents<diji::SpriteRenderComponent>("graphics/star.png", sf::Vector2i{ 50,50 }, 4, 0.035f);
    m_ItemTemplateUPtr->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    m_ItemTemplateUPtr->AddComponents<StarPower>(IPowerUp::PowerUpType::Star, false);
}