#include "SelectorControls.h"

#include "Selector.h"
#include "../../Helpers/WorldBuilder.h"
#include "../../Singletons/LevelEditorManager.h"
#include "Engine/Components/Render.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"
#include "Engine/Components/TextureComp.h"

void superMarioBros::SelectorControls::Init()
{
    m_TransformCompPtr = GetOwner()->GetRootComponent();

    for (MenuItem* menuItem : m_MenuItems)
        menuItem->SetMenuArrow(m_MenuArrowRef);

    m_RenderCompPtr = GetOwner()->GetComponent<diji::Render>();
    
    auto canvasSelector = std::make_unique<diji::GameObject>();
    canvasSelector->AddComponent<diji::TextureComp>("graphics/squareWhiteSmaller50.png");
    const auto& renderComp = canvasSelector->AddComponent<diji::Render>(m_MenuRenderRatio);
    renderComp->SetRenderLayer(2);
    m_CanvasSelector = diji::SceneManager::GetInstance().SpawnGameObject("CanvasSelector", std::move(canvasSelector), sf::Vector2f{ 0.f, 0.f });
    diji::SceneManager::GetInstance().SetGameObjectAsCanvasObject(m_CanvasSelector);
    m_CanvasSelector->SetActive(false);
}

void superMarioBros::SelectorControls::Start()
{
    m_SelectorRef = GetOwner()->GetComponent<Selector>();
    
    diji::TimerManager::GetInstance().DelayUntilNextTick([&]
    {
        m_MenuYPosition = m_MenuItems[0]->GetOwner()->GetObjectPosition().y;
        m_MenuEntries.clear();

        for (MenuItem* menuItem : m_MenuItems)
        {
            const int xPos = static_cast<int>(menuItem->GetOwner()->GetObjectPosition().x);
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

        m_TransformCompPtr->AddWorldOffset(sf::Vector2f{ direction.x * WorldBuilder::WorldSettings::TILE_SIZE, direction.y * WorldBuilder::WorldSettings::TILE_SIZE });
        m_SelectorRef->UpdatePreviewItems();
        m_MoveTimerHandle = diji::TimerManager::GetInstance().SetTimer(this, 
            [this, direction]()
            {
                if (!CanMove(direction))
                    return;

                m_TransformCompPtr->AddWorldOffset(sf::Vector2f{ direction.x * WorldBuilder::WorldSettings::TILE_SIZE, direction.y * WorldBuilder::WorldSettings::TILE_SIZE });
                m_SelectorRef->UpdatePreviewItems();
            },
            DELAY_BETWEEN_MOVE, true, INITIAL_DELAY_BEFORE_MOVE
        );

        return;
    }

    diji::TimerManager::GetInstance().ClearTimer(m_MoveTimerHandle);
}

void superMarioBros::SelectorControls::OpenSaveMenu()
{
    if (m_DisableMovement)
    {
        ClearOutOfSpecialMenu();
        return;
    }
    
    EnterMenu();
    m_IsInMenu = true;
    ChooseMenuItem(1);
    SelectCurrentMenuItem();
}

void superMarioBros::SelectorControls::OpenBlocksMenu()
{
    if (m_DisableMovement)
    {
        ClearOutOfSpecialMenu();
        return;
    }
    
    EnterMenu();
    m_IsInMenu = true;
    ChooseMenuItem(2);
    SelectCurrentMenuItem();
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
    if (m_IsInMenu == false)
    {
        m_SelectorRef->TryHoldPlaceItem();
        return;
    }
    
    if  (!m_DisableMovement)
        return;

    if (m_MenuEntries[m_CurrentMenuIndex].menuItemPtr->Return())
        return;
        
    ResetSpecialMenu();
}

void superMarioBros::SelectorControls::TrySetCanvasSelector(const sf::Vector2f& pos) const
{
    m_CanvasSelector->SetObjectPosition(pos);
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
    
    m_CanvasSelector->SetObjectPosition(sf::Vector2f{ static_cast<float>(m_MenuEntries[m_CurrentMenuIndex].xPosition), m_MenuYPosition });

    return true;
}

bool superMarioBros::SelectorControls::CheckForMenuMovementY(const sf::Vector2f& dir)
{
    const sf::Vector2f pos = m_TransformCompPtr->GetWorldPosition();
    if (m_IsInMenu && dir.y > 0) // Exit Menu
    {
        m_CanvasSelector->SetActive(false);
        m_RenderCompPtr->EnableRender();
        m_SelectorRef->EnablePreview();
        
        m_IsInMenu = false;
        return true;
    }

    if (!m_IsInMenu && dir.y < 0 && diji::Helpers::AreFloatEqual(pos.y, m_ArenaBoundsVertical.x))
    {
        EnterMenu();
        
        m_IsInMenu = true;
        return true;
    }
    
    return m_IsInMenu;
}

void superMarioBros::SelectorControls::EnterMenu()
{
    const sf::Vector2f pos = m_TransformCompPtr->GetWorldPosition();
    const float menuXPos = static_cast<float>(ClosestMenuItemPosition(static_cast<int>(pos.x)));

    m_CanvasSelector->SetObjectPosition(sf::Vector2f{ menuXPos, m_MenuYPosition });
    m_CanvasSelector->SetActive(true);
    m_RenderCompPtr->DisableRender();
    m_SelectorRef->DisablePreview();
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

void superMarioBros::SelectorControls::ChooseMenuItem(int index)
{
    if (m_MenuEntries.empty())
        return;

    index = std::clamp(index, 0, static_cast<int>(m_MenuEntries.size()) - 1);
    m_CurrentMenuIndex = index;
    m_CanvasSelector->SetObjectPosition(sf::Vector2f{static_cast<float>(m_MenuEntries[index].xPosition),m_MenuYPosition});
}
