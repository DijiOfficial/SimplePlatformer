#include "OneUpBlock.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Core/GameObject.h"
#include "../../PowerUps/MushroomScript.h"

void thomasWasLate::OneUpBlock::Init()
{
    BaseBlock::Init();

    m_AudioName = "smb_powerup_appears.wav";
}

void thomasWasLate::OneUpBlock::CreateItemTemplate()
{
    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_ItemTemplateUPtr->AddComponents<diji::Transform>(0, 0);
    m_ItemTemplateUPtr->AddComponents<diji::TextureComp>("graphics/oneUpMushroom.png");
    m_ItemTemplateUPtr->AddComponents<diji::Render>();
    m_ItemTemplateUPtr->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    m_ItemTemplateUPtr->AddComponents<MushroomScript>(IPowerUp::PowerUpType::OneUpMushroom, true);
}
