#include "BlockSelector.h"

#include "../Selector.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"

std::unordered_map<int, superMarioBros::BlockSelector::eBlockType> superMarioBros::BlockSelector::m_BlockTypeMap =
{
    { 0,  Floor },
    { 1,  PipeTopLeft },
    { 2,  PipeTopRight },
    { 3,  FlagPoleTop },
    
    { 4,  Wall },
    { 5,  PipeBottomLeft },
    { 6,  PipeBottomRight },
    { 7,  FlagPole },

    { 8,  PipeSideTopLeft },
    { 9,  PipeSideTopMiddle },
    { 10, PipeSideTopRight },
    { 11, PlaceHolder },

    { 12, PipeSideBottomLeft },
    { 13, PipeSideBottomMiddle },
    { 14, PipeSideBottomRight },
    { 15, PlaceHolder2 },

    { 16, LuckyBlock },
    { 17, BreakableBlock },
    { 18, Coin },
    { 19, PlaceHolder3 },
};

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
    m_SelectorPosition = m_SelectorGO->GetObjectPosition();
    UpdateSelectorPosition();

    return true;
}

void superMarioBros::BlockSelector::CloseMenu()
{
    m_ChildChoiceGO->SetActive(false);
    m_SelectorGO->SetObjectPosition(m_SelectorPosition);
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
            m_BlockTypePositionsMap[index++] = logicalPos;
        }
    }
}

void superMarioBros::BlockSelector::UpdateSelectorPosition()
{
    m_SelectorGO->SetObjectPosition(m_BlockTypePositionsMap[m_CurrentBlockIndex]);
}

