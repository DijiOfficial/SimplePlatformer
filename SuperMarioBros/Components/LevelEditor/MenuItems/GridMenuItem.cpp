#include "GridMenuItem.h"

#include "../SelectorControls.h"
#include "../../../Helpers/WorldBuilder.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"

void superMarioBros::GridMenuItem::Start()
{
    m_ChildChoiceGO = GetOwner()->GetRootComponent()->GetChildren().at(0)->GetGameObject();
    m_ChildChoiceGO->SetActive(false);

    diji::TimerManager::GetInstance().DelayUntilNextTick([&]
    {
        SetAllGridPositions();
    });
}

bool superMarioBros::GridMenuItem::ActivateMenu()
{
    m_ChildChoiceGO->SetActive(true);
    UpdateSelectorPosition();

    return true;
}

void superMarioBros::GridMenuItem::CloseMenu()
{
    m_ChildChoiceGO->SetActive(false);
}

void superMarioBros::GridMenuItem::Move(const sf::Vector2f& direction, const bool isStart)
{
    if (isStart == false) return;

    int row = m_CurrentIndex / m_GridWidth;
    int col = m_CurrentIndex % m_GridWidth;

    if (direction.x > 0.0f)
    {
        col = (col + 1) % m_GridWidth;
    }
    else if (direction.x < 0.0f)
    {
        col = (col - 1 + m_GridWidth) % m_GridWidth;
    }
    else if (direction.y > 0.0f)
    {
        row = (row + 1) % m_GridHeight;
    }
    else if (direction.y < 0.0f)
    {
        row = (row - 1 + m_GridHeight) % m_GridHeight;
    }

    m_CurrentIndex = row * m_GridWidth + col;
    UpdateSelectorPosition();
}

bool superMarioBros::GridMenuItem::Return()
{
    CloseMenu();
    return false;
}

void superMarioBros::GridMenuItem::UpdateSelectorPosition()
{
    m_SelectorControls->TrySetCanvasSelector(m_GridPositionsMap[m_CurrentIndex]);
}

void superMarioBros::GridMenuItem::SetAllGridPositions()
{
    constexpr float tileSize = WorldBuilder::WorldSettings::TILE_SIZE;
    constexpr float halfTileSize = tileSize * 0.5f;

    const auto& sceneManager = diji::SceneManager::GetInstance();

    const auto& size = m_ChildChoiceGO->GetComponent<diji::TextureComp>()->GetSize();
    const auto& pos  = sceneManager.GetWorldPositionFromScreen(m_ChildChoiceGO->GetObjectPosition());

    m_GridWidth  = static_cast<int>(static_cast<float>(size.x) / tileSize);
    m_GridHeight = static_cast<int>(static_cast<float>(size.y) / tileSize);

    const sf::Vector2f topLeft = pos - sf::Vector2f{ size.x * 0.5f, size.y * 0.5f };

    int index = 0;
    for (int row = 0; row < m_GridHeight; ++row)
    {
        for (int col = 0; col < m_GridWidth; ++col)
        {
            const sf::Vector2f logicalPos = topLeft + sf::Vector2f(static_cast<float>(col) * tileSize + halfTileSize, static_cast<float>(row) * tileSize + halfTileSize);
            m_GridPositionsMap[index++] = sceneManager.GetScreenPosition(logicalPos);
        }
    }
}
