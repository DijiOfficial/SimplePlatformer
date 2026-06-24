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
    m_LevelNameObjects.reserve(m_LevelNames.size() + 1);
    m_LevelData.reserve(m_LevelNames.size() + 1);

    LoadMenuItems();
    CloseMenu();
}

bool superMarioBros::LoadMenu::ActivateMenu()
{
    for (const auto level : m_LevelNameObjects)
        level->SetActive(true);

    m_SpecialMenuIndex = 0;

    m_MenuArrow->SetObjectPosition(sf::Vector2f{ m_LevelData[m_SpecialMenuIndex].Position });
    m_MenuArrow->SetActive(true);
    return true;
}

void superMarioBros::LoadMenu::CloseMenu()
{
    for (const auto level : m_LevelNameObjects)
        level->SetActive(false);

    m_MenuArrow->SetActive(false);
}

void superMarioBros::LoadMenu::Move(const sf::Vector2f& direction, const bool isStart)
{
    if (direction.y == 0.0f || !isStart)
        return;

    const int count = static_cast<int>(m_LevelData.size());
    const int delta = direction.y > 0.0f ? 1 : -1;
    m_SpecialMenuIndex = (m_SpecialMenuIndex + delta + count) % count;
    
    m_MenuArrow->SetObjectPosition(m_LevelData[m_SpecialMenuIndex].Position);
}

bool superMarioBros::LoadMenu::Select()
{
    if (m_SpecialMenuIndex == 0)
        LevelEditorManager::GetInstance().LoadNewLevel();
    else
        LevelEditorManager::GetInstance().LoadLevel(m_LevelData[m_SpecialMenuIndex].Name);
    
    return false;
}

void superMarioBros::LoadMenu::AddNewName(const std::string& newLevelName)
{
    m_LevelNames.emplace_back(newLevelName);

    m_LevelNameObjects = std::vector<diji::GameObject*>();
    m_LevelData = std::vector<LoadingLevelData>();
    m_LevelNameObjects.reserve(m_LevelNames.size() + 1);
    m_LevelData.reserve(m_LevelNames.size() + 1);

    LoadMenuItems();
    CloseMenu();
}

void superMarioBros::LoadMenu::LoadMenuItems()
{
    const float xPos = m_TransformCompPtr->GetWorldPosition().x + INITIAL_SPACING;
    float yPos = m_TransformCompPtr->GetWorldPosition().y + INITIAL_SPACING;

    yPos += ITEM_SPACING;
    CreateMenuItem("New Level", xPos, yPos);
    
    for (const std::string& levelName : m_LevelNames)
    {
        yPos += ITEM_SPACING;
        CreateMenuItem(levelName, xPos, yPos);
    }
}

void superMarioBros::LoadMenu::CreateMenuItem(const std::string& levelName, const float xPos, const float yPos)
{
    const auto& levelNameObject = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_LevelName", m_ItemTemplateUPtr.get(), sf::Vector2f{ xPos, yPos });
    m_LevelNameObjects.emplace_back(levelNameObject);

    auto& textComp = levelNameObject->GetComponent<diji::TextComp>()->GetText();
    textComp.setString(levelName);

    const sf::FloatRect bounds = textComp.getGlobalBounds();
    const sf::Vector2f pos = sf::Vector2f{ levelNameObject->GetRootComponent()->GetWorldPosition().x - bounds.size.x * 0.5f - SELECTOR_SPACING, yPos };
    m_LevelData.emplace_back(LoadingLevelData{ .Position= pos, .Name= levelName });
}
