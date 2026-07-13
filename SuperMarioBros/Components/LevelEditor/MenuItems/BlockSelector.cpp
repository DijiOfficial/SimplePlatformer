#include "BlockSelector.h"

#include "../Selector.h"
#include "../SelectorControls.h"
#include "../../../Singletons/LevelEditorManager.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"

void superMarioBros::BlockSelector::Start()
{
    m_ChildChoiceGO = GetOwner()->GetRootComponent()->GetChildren().at(0)->GetGameObject();
    m_ChildChoiceGO->SetActive(false);

    diji::TimerManager::GetInstance().DelayUntilNextTick([&]
    {
        SetAllBlocksPositions();
    });
}

bool superMarioBros::BlockSelector::ActivateMenu()
{
    m_ChildChoiceGO->SetActive(true);
    UpdateSelectorPosition();

    return true;
}

void superMarioBros::BlockSelector::CloseMenu()
{
    m_ChildChoiceGO->SetActive(false);
}

void superMarioBros::BlockSelector::Move(const sf::Vector2f& direction, const bool isStart)
{
    if (!isStart) return;

    int row = m_CurrentBlockIndex / m_GridWidth;
    int col = m_CurrentBlockIndex % m_GridWidth;

    if (direction.x > 0.f)
    {
        col = (col + 1) % m_GridWidth;
    }
    else if (direction.x < 0.f)
    {
        col = (col - 1 + m_GridWidth) % m_GridWidth;
    }
    else if (direction.y > 0.f)
    {
        row = (row + 1) % m_GridHeight;
    }
    else if (direction.y < 0.f)
    {
        row = (row - 1 + m_GridHeight) % m_GridHeight;
    }

    m_CurrentBlockIndex = row * m_GridWidth + col;
    UpdateSelectorPosition();
}

bool superMarioBros::BlockSelector::Select()
{
    m_Selector->ActivateBackgroundTexture();
    const int row = m_CurrentBlockIndex / m_GridWidth;
    const int col = m_CurrentBlockIndex % m_GridWidth;
    m_Selector->SetFramePosition(col, row);
    
    return false;
}

bool superMarioBros::BlockSelector::Return()
{
    CloseMenu();
    return false;
}

void superMarioBros::BlockSelector::SelectNextOrPreviousBlock(const bool isNext)
{
    if (isNext)
    {
        m_CurrentBlockIndex =(m_CurrentBlockIndex + 1) % (m_GridWidth * m_GridHeight);
    }
    else
    {
        const int count = m_GridWidth * m_GridHeight;
        m_CurrentBlockIndex = (m_CurrentBlockIndex - 1 + count) % count;
    }

    UpdateSelectorPosition();
    (void)Select();
}

void superMarioBros::BlockSelector::CopyBlockHoveredPosition() const
{
    m_Selector->ActivateBackgroundTexture();
    const sf::Vector2f& pos = m_Selector->GetOwner()->GetObjectPosition();

    const int row = static_cast<int>(pos.y - 25.f) / 50;
    const int col = static_cast<int>(pos.x - 25.f) / 50;
    const auto& grid = m_Selector->GetGridPosFromChar(LevelEditorManager::GetInstance().GetLevelInfoAtPos(col, row));

    m_Selector->SetFramePosition(grid.x, grid.y);
}

void superMarioBros::BlockSelector::SetAllBlocksPositions()
{
    constexpr int tileSize = 50;
    constexpr float halfTileSize = tileSize * 0.5f;

    const auto& sceneManager = diji::SceneManager::GetInstance();

    const auto& size = m_ChildChoiceGO->GetComponent<diji::TextureComp>()->GetSize();
    const auto& pos  = sceneManager.GetWorldPositionFromScreen(m_ChildChoiceGO->GetObjectPosition());

    m_GridWidth  = static_cast<int>(size.x) / tileSize;
    m_GridHeight = static_cast<int>(size.y) / tileSize;

    const sf::Vector2f topLeft = pos - sf::Vector2f{ size.x * 0.5f, size.y * 0.5f };

    int index = 0;
    for (int row = 0; row < m_GridHeight; ++row)
    {
        for (int col = 0; col < m_GridWidth; ++col)
        {
            const sf::Vector2f logicalPos = topLeft + sf::Vector2f(col * tileSize + halfTileSize, row * tileSize + halfTileSize);
            m_BlockTypePositionsMap[index++] = sceneManager.GetScreenPosition(logicalPos);
        }
    }
}

void superMarioBros::BlockSelector::UpdateSelectorPosition()
{
    m_SelectorControls->TrySetCanvasSelector(m_BlockTypePositionsMap[m_CurrentBlockIndex]);
}

