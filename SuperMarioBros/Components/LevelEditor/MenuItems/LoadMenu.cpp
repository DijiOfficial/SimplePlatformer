#include "LoadMenu.h"

#include "../../../Singletons/LevelEditorManager.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::LoadMenu::Init()
{
    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_ItemTemplateUPtr->AddComponent<diji::TextComp>("level name", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    m_ItemTemplateUPtr->GetComponent<diji::TextComp>()->GetText().setCharacterSize(25);
    m_ItemTemplateUPtr->GetComponent<diji::TextComp>()->SetCentered(true);
    m_ItemTemplateUPtr->AddComponent<diji::Render>();

    m_TransformCompPtr = GetOwner()->GetRootComponent();
}

void superMarioBros::LoadMenu::Start()
{
    m_LevelNames = LevelEditorManager::GetInstance().GetLevelNames();
}

superMarioBros::MenuItem::MenuInfo superMarioBros::LoadMenu::ActivateMenu()
{
    const float xPos = m_TransformCompPtr->GetWorldPosition().x + INITIAL_SPACING;
    float yPos = m_TransformCompPtr->GetWorldPosition().y + INITIAL_SPACING;

    std::vector<LoadingLevelData> levelData;
    levelData.reserve(m_LevelNames.size());
    
    for (const std::string& levelName : m_LevelNames)
    {
        yPos += ITEM_SPACING;

        const auto& levelNameObject = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_LevelName", m_ItemTemplateUPtr.get(), sf::Vector2f{ xPos, yPos });

        auto& textComp = levelNameObject->GetComponent<diji::TextComp>()->GetText();
        textComp.setString(levelName);

        const sf::FloatRect bounds = textComp.getGlobalBounds();
        const sf::Vector2f pos = sf::Vector2f{ levelNameObject->GetRootComponent()->GetWorldPosition().x - bounds.size.x * 0.5f - SELECTOR_SPACING, yPos };
        levelData.emplace_back(LoadingLevelData{ .Position= pos, .Name= levelName });
    }
    
    return MenuInfo{ .ShouldLockControls= true, .MenuType= eMenuType::Load ,.Data= levelData };
}

void superMarioBros::LoadMenu::CloseMenu()
{
}
