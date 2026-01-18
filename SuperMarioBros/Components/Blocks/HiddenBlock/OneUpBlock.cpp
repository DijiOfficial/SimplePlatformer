#include "OneUpBlock.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Core/GameObject.h"
#include "../../PowerUps/MushroomScript.h"

void superMarioBros::OneUpBlock::Init()
{
    BaseBlock::Init();

    m_AudioName = "smb_powerup_appears.wav";
}

void superMarioBros::OneUpBlock::CreateItemTemplate()
{
    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_ItemTemplateUPtr->AddComponent<diji::TextureComp>("graphics/oneUpMushroom.png");
    m_ItemTemplateUPtr->AddComponent<diji::Render>();
    m_ItemTemplateUPtr->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    m_ItemTemplateUPtr->AddComponent<MushroomScript>(IPowerUp::PowerUpType::OneUpMushroom, true);
}
