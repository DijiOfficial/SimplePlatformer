#include "EnemySelector.h"
#include "Engine/Core/GameObject.h" // need this for reasons?
#include "../Selector.h"

bool superMarioBros::EnemySelector::Select()
{
    m_Selector->ActivateBackgroundTexture();
    m_Selector->SelectEnemy(GetCurrentGridPos());

    return false;
}

bool superMarioBros::EnemySelector::IsCellSelectable(const int index) const
{
    return m_Selector->IsEnemyCell(sf::Vector2i{ index % m_GridWidth, index / m_GridWidth });
}
