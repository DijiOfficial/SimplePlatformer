#include "StarBlock.h"
#include "Engine/Collision/Collider.h"
#include "../../PowerUps/StarPower.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"

void superMarioBros::StarBlock::Init()
{
    BaseBlock::Init();

    m_AudioName = "smb_powerup_appears.wav";
}

void superMarioBros::StarBlock::CreateItemTemplate()
{
    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_ItemTemplateUPtr->AddComponent<diji::SpriteRenderComponent>("graphics/star.png", sf::Vector2i{ 50,50 }, 4, 0.035f);
    m_ItemTemplateUPtr->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    m_ItemTemplateUPtr->AddComponent<StarPower>(IPowerUp::PowerUpType::Star, false);
}