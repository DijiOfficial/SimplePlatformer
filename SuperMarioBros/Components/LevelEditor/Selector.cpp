#include "Selector.h"


#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/TimerManager.h"

void superMarioBros::Selector::Init()
{
    m_TransformCompPtr = GetOwner()->GetRootComponent();
    std::ranges::sort(m_MenuPositions);
}

void superMarioBros::Selector::Move(const sf::Vector2f& direction, const bool isStart)
{
    if (isStart)
    {
        diji::TimerManager::GetInstance().ClearTimer(m_MoveTimerHandle);

        if (!CanMove(direction))
            return;

        m_TransformCompPtr->AddWorldOffset(sf::Vector2f{ direction.x * TILE_SIZE, direction.y * TILE_SIZE });
        m_MoveTimerHandle = diji::TimerManager::GetInstance().SetTimer(
            [this, direction]()
            {
                if (!CanMove(direction))
                    return;

                m_TransformCompPtr->AddWorldOffset(sf::Vector2f{ direction.x * TILE_SIZE, direction.y * TILE_SIZE });
            },
            DELAY_BETWEEN_MOVE,
            true,
            INITIAL_DELAY_BEFORE_MOVE
        );

        return;
    }

    diji::TimerManager::GetInstance().ClearTimer(m_MoveTimerHandle);
}

bool superMarioBros::Selector::CanMove(const sf::Vector2f& dir)
{
    const bool isMovingX = !diji::Helpers::AreFloatEqual(dir.x, 0.0f);
    if (isMovingX && CheckForMenuMovementX(dir))
        return false;
    
    const bool isMovingY = !diji::Helpers::AreFloatEqual(dir.y, 0.0f);
    if (isMovingY && CheckForMenuMovementY(dir))
        return false;
    
    const auto pos = m_TransformCompPtr->GetWorldPosition();
    const bool canMoveX = (dir.x < 0 && pos.x <= m_ArenaBoundsHorizontal.x) ||(dir.x > 0 && pos.x >= m_ArenaBoundsHorizontal.y);
    const bool canMoveY = (dir.y > 0 && pos.y >= m_ArenaBoundsVertical.y) || (dir.y < 0 && pos.y <= m_ArenaBoundsVertical.x);
    const bool canMove = !(canMoveX || canMoveY);
    
    return canMove;
}

bool superMarioBros::Selector::CheckForMenuMovementX(const sf::Vector2f& dir)
{
    if (m_IsInMenu)
    {
        // move to next target
        return true;
    }

    return false;
}

bool superMarioBros::Selector::CheckForMenuMovementY(const sf::Vector2f& dir)
{
    if (m_IsInMenu && dir.y > 0)
    {
        // todo: make sure x position is valid (multiple of 50 with the 25 offset)
        m_TransformCompPtr->AddWorldOffset(sf::Vector2f{ m_TransformCompPtr->GetWorldPosition().x , m_ArenaBoundsVertical.y });
        m_IsInMenu = false;
        return true;
    }

    if (!m_IsInMenu && dir.y < 0)
    {
        EnterMenu();
        
        m_IsInMenu = true;
        return true;
    }
    
    return false;
}

void superMarioBros::Selector::EnterMenu()
{
    const sf::Vector2f pos = m_TransformCompPtr->GetWorldPosition();
    const float menuXPos = ClosestMenuItemPosition(static_cast<int>(pos.x));
        
    m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ menuXPos, MENU_Y_POSITION });
}

int superMarioBros::Selector::ClosestMenuItemPosition(const int x)
{
    if (m_MenuPositions.empty())
        return x;

    const auto it = std::ranges::lower_bound(m_MenuPositions, x);
    if (it != m_MenuPositions.end() && *it == x)
    {
        m_CurrentMenuIndex = static_cast<int>(it - m_MenuPositions.begin());
        return *it;
    }
    
    if (it == m_MenuPositions.begin())
    {
        m_CurrentMenuIndex = 0;
        return m_MenuPositions[0];
    }

    if (it == m_MenuPositions.end())
    {
        m_CurrentMenuIndex = static_cast<int>(m_MenuPositions.size() - 1);
        return m_MenuPositions.back();
    }

    const int idx = static_cast<int>(it - m_MenuPositions.begin());
    const int lowerValue = *it;
    const int higherValue = *(it - 1);
    const int distanceA = x - lowerValue;
    const int distanceB = higherValue - x;

    if (distanceA <= distanceB)
    {
        m_CurrentMenuIndex = idx - 1;
        return lowerValue;
    }

    m_CurrentMenuIndex = idx;
    return higherValue;
}
