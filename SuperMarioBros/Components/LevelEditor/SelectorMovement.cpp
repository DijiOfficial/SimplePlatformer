#include "SelectorMovement.h"

#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"

void superMarioBros::SelectorMovement::Init()
{
    m_TransformCompPtr = GetOwner()->GetRootComponent();
}

void superMarioBros::SelectorMovement::Start()
{
    const auto& sceneManager = diji::SceneManager::GetInstance();
    
    diji::TimerManager::GetInstance().DelayUntilNextTick([&]
    {
        m_MenuYPosition = sceneManager.GetWorldPositionFromScreen(m_MenuItemTransforms[0]->GetWorldPosition()).y;
        for (diji::Transform* menuItem : m_MenuItemTransforms)
        {
            m_MenuPositions.push_back(static_cast<int>(sceneManager.GetWorldPositionFromScreen(menuItem->GetWorldPosition()).x));
        }
        std::ranges::sort(m_MenuPositions);
    });
}

void superMarioBros::SelectorMovement::Move(const sf::Vector2f& direction, const bool isStart)
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

bool superMarioBros::SelectorMovement::CanMove(const sf::Vector2f& dir)
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

bool superMarioBros::SelectorMovement::CheckForMenuMovementX(const sf::Vector2f& dir)
{
    if (!m_IsInMenu || m_MenuPositions.empty())
        return false;
    
    const int count = static_cast<int>(m_MenuPositions.size());
    const int delta = dir.x > 0.0f ? 1 : -1;

    m_CurrentMenuIndex = (m_CurrentMenuIndex + delta + count) % count;
    
    m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ static_cast<float>(m_MenuPositions[m_CurrentMenuIndex]), m_TransformCompPtr->GetWorldPosition().y });
    
    return true;
}

bool superMarioBros::SelectorMovement::CheckForMenuMovementY(const sf::Vector2f& dir)
{
    const sf::Vector2f pos = m_TransformCompPtr->GetWorldPosition();
    if (m_IsInMenu && dir.y > 0) // Exit Menu
    {
        const float snappedX = 25.0f + std::round((pos.x - 25.0f) / 50.0f) * 50.0f;

        m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ snappedX, m_ArenaBoundsVertical.x });
        m_IsInMenu = false;
        return true;
    }

    if (!m_IsInMenu && dir.y < 0 && diji::Helpers::AreFloatEqual(pos.y, m_ArenaBoundsVertical.x))
    {
        EnterMenu();
        
        m_IsInMenu = true;
        return true;
    }
    
    return false;
}

void superMarioBros::SelectorMovement::EnterMenu()
{
    const sf::Vector2f pos = m_TransformCompPtr->GetWorldPosition();
    const float menuXPos = static_cast<float>(ClosestMenuItemPosition(static_cast<int>(pos.x)));
        
    m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ menuXPos, m_MenuYPosition });
}

int superMarioBros::SelectorMovement::ClosestMenuItemPosition(const int x)
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
