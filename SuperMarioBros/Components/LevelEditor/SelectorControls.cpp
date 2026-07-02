#include "SelectorControls.h"

#include "Selector.h"
#include "../../Singletons/LevelEditorManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"

void superMarioBros::SelectorControls::Init()
{
    m_TransformCompPtr = GetOwner()->GetRootComponent();

    for (MenuItem* menuItem : m_MenuItems)
        menuItem->SetMenuArrow(m_MenuArrowRef);
}

void superMarioBros::SelectorControls::Start()
{
    m_SelectorRef = GetOwner()->GetComponent<Selector>();
    
    const auto& sceneManager = diji::SceneManager::GetInstance();
    diji::TimerManager::GetInstance().DelayUntilNextTick([&]
    {
        m_MenuYPosition = sceneManager.GetWorldPositionFromScreen(m_MenuItems[0]->GetOwner()->GetObjectPosition()).y;
        m_MenuEntries.clear();

        for (MenuItem* menuItem : m_MenuItems)
        {
            const int xPos = static_cast<int>(sceneManager.GetWorldPositionFromScreen(menuItem->GetOwner()->GetObjectPosition()).x);
            m_MenuEntries.push_back({ .xPosition= xPos, .menuItemPtr= menuItem });
        }

        std::ranges::sort(m_MenuEntries,
            [](const MenuEntry& a, const MenuEntry& b)
            {
                return a.xPosition < b.xPosition;
            });
    });
}

void superMarioBros::SelectorControls::Move(const sf::Vector2f& direction, const bool isStart)
{
    if  (m_DisableMovement)
    {
        m_MenuEntries[m_CurrentMenuIndex].menuItemPtr->Move(direction, isStart);
        return;
    }
    
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
            DELAY_BETWEEN_MOVE, true, INITIAL_DELAY_BEFORE_MOVE
        );

        return;
    }

    diji::TimerManager::GetInstance().ClearTimer(m_MoveTimerHandle);
}

void superMarioBros::SelectorControls::SelectCurrentMenuItem()
{
    if (m_IsInMenu == false)
    {
        m_SelectorRef->TryPlaceItem();
        return;
    }
    
    if (m_MenuEntries.empty())
        return;

    if (m_DisableMovement)
    {
        if (m_MenuEntries[m_CurrentMenuIndex].menuItemPtr->Select())
            return;

        ResetSpecialMenu();
        return;
    }

    m_DisableMovement = m_MenuEntries[m_CurrentMenuIndex].menuItemPtr->ActivateMenu();
}

void superMarioBros::SelectorControls::ClearOutOfSpecialMenu()
{
    if  (!m_DisableMovement)
        return;

    if (m_MenuEntries[m_CurrentMenuIndex].menuItemPtr->Return())
        return;
        
    ResetSpecialMenu();
}

bool superMarioBros::SelectorControls::CanMove(const sf::Vector2f& dir)
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

bool superMarioBros::SelectorControls::CheckForMenuMovementX(const sf::Vector2f& dir)
{
    if (!m_IsInMenu || m_MenuEntries.empty())
        return false;
    
    const int count = static_cast<int>(m_MenuEntries.size());
    const int delta = dir.x > 0.0f ? 1 : -1;

    m_CurrentMenuIndex = (m_CurrentMenuIndex + delta + count) % count;
    
    m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ static_cast<float>(m_MenuEntries[m_CurrentMenuIndex].xPosition), m_TransformCompPtr->GetWorldPosition().y });
    
    return true;
}

bool superMarioBros::SelectorControls::CheckForMenuMovementY(const sf::Vector2f& dir)
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

void superMarioBros::SelectorControls::EnterMenu()
{
    const sf::Vector2f pos = m_TransformCompPtr->GetWorldPosition();
    const float menuXPos = static_cast<float>(ClosestMenuItemPosition(static_cast<int>(pos.x)));
        
    m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ menuXPos, m_MenuYPosition });
}

void superMarioBros::SelectorControls::ResetSpecialMenu()
{
    m_DisableMovement = false;
    m_IsInMenu = true;
    m_MenuEntries[m_CurrentMenuIndex].menuItemPtr->CloseMenu();

    if (m_CurrentMenuIndex == 2)
        Move(sf::Vector2f{ 0.0f, 1.0f } ,true);
}

int superMarioBros::SelectorControls::ClosestMenuItemPosition(const int x)
{
    if (m_MenuEntries.empty())
        return x;

    const auto it = std::ranges::lower_bound(m_MenuEntries,x,{},&MenuEntry::xPosition);

    if (it != m_MenuEntries.end() && it->xPosition == x)
    {
        m_CurrentMenuIndex = static_cast<int>(it - m_MenuEntries.begin());
        return it->xPosition;
    }

    if (it == m_MenuEntries.begin())
    {
        m_CurrentMenuIndex = 0;
        return m_MenuEntries[0].xPosition;
    }

    if (it == m_MenuEntries.end())
    {
        m_CurrentMenuIndex = static_cast<int>(m_MenuEntries.size() - 1);
        return m_MenuEntries.back().xPosition;
    }

    const int idx = static_cast<int>(it - m_MenuEntries.begin());

    const MenuEntry& higherEntry = *it;
    const MenuEntry& lowerEntry = *(it - 1);

    const int distanceA = x - lowerEntry.xPosition;
    const int distanceB = higherEntry.xPosition - x;

    if (distanceA <= distanceB)
    {
        m_CurrentMenuIndex = idx - 1;
        return lowerEntry.xPosition;
    }

    m_CurrentMenuIndex = idx;
    return higherEntry.xPosition;
}
