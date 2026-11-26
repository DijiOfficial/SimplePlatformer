#include "BlockSelector.h"

#include "../Selector.h"
#include "../../../Singletons/LevelEditorManager.h"
#include "Engine/Core/GameObject.h"

bool superMarioBros::BlockSelector::Select()
{
    m_Selector->ActivateBackgroundTexture();
    const sf::Vector2i gridPos = GetCurrentGridPos();
    m_Selector->SetFramePosition(gridPos.x, gridPos.y);

    return false;
}

void superMarioBros::BlockSelector::SelectNextOrPreviousBlock(const bool isNext)
{
    if (isNext)
    {
        m_CurrentIndex =(m_CurrentIndex + 1) % (m_GridWidth * m_GridHeight);
    }
    else
    {
        const int count = m_GridWidth * m_GridHeight;
        m_CurrentIndex = (m_CurrentIndex - 1 + count) % count;
    }

    UpdateSelectorPosition();
    (void)Select();
}

void superMarioBros::BlockSelector::CopyBlockHoveredPosition() const
{
    m_Selector->ActivateBackgroundTexture();
    const sf::Vector2f& pos = m_Selector->GetOwner()->GetObjectPosition();

    const int row = static_cast<int>(pos.y - 25.0f) / 50;
    const int col = static_cast<int>(pos.x - 25.0f) / 50;
    const auto& grid = m_Selector->GetGridPosFromChar(LevelEditorManager::GetInstance().GetLevelInfoAtPos(col, row));

    m_Selector->SetFramePosition(grid.x, grid.y);
}
