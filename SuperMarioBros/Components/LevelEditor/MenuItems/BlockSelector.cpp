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

void superMarioBros::BlockSelector::CopyBlockHoveredPosition() const
{
    m_Selector->ActivateBackgroundTexture();
    const sf::Vector2f& pos = m_Selector->GetOwner()->GetObjectPosition();

    const int row = static_cast<int>(pos.y - 25.0f) / 50;
    const int col = static_cast<int>(pos.x - 25.0f) / 50;
    m_Selector->SelectItemFromChar(LevelEditorManager::GetInstance().GetLevelInfoAtPos(col, row));
}
