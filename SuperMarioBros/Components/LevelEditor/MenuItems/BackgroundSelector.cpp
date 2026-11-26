#include "BackgroundSelector.h"

#include "../../../Singletons/LevelEditorManager.h"

bool superMarioBros::BackgroundSelector::ActivateMenu()
{
    const std::string& currentBackground = LevelEditorManager::GetInstance().GetCurrentMapInfo().background;
    for (const auto& [gridPos, background] : m_GridToBackgroundMap)
    {
        if (background == currentBackground)
        {
            SetCurrentGridPos(gridPos);
            break;
        }
    }

    return GridMenuItem::ActivateMenu();
}

bool superMarioBros::BackgroundSelector::Select()
{
    const sf::Vector2i gridPos = GetCurrentGridPos();
    const auto it = m_GridToBackgroundMap.find(gridPos);
    if (it == m_GridToBackgroundMap.end())
        throw std::runtime_error("Invalid background selected at grid position: " + std::to_string(gridPos.x) + ", " + std::to_string(gridPos.y));

    LevelEditorManager::GetInstance().SetBackground(it->second);

    return false;
}
