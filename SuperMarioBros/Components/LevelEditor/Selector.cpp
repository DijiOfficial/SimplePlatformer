#include "Selector.h"

#include <ranges>

#include "SelectorControls.h"
#include "../../Singletons/GameManager.h"
#include "../../Singletons/LevelEditorManager.h"
#include "../Backgrounds/BackgroundHandler.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::Selector::Init()
{
    LevelEditorManager::GetInstance().Init();

    const sf::Vector2f startPos = LevelEditorManager::GetInstance().m_SelectorPosition;
    if (!diji::Helpers::AreVectorsEqual(startPos, sf::Vector2f{ 0.f, 0.f }))
    {
        GetOwner()->SetObjectPosition(startPos);
    }

    CreateBackgroundTexture();
    
    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_TemplateSpriteRenderComp = m_ItemTemplateUPtr->AddComponent<diji::SpriteRenderComponent>("graphics/tiles_sheet_selection.png", sf::Vector2i{ 50, 50 }, 1, 1.0f);
}

void superMarioBros::Selector::Start()
{
    m_TransformCompPtr = GetOwner()->GetRootComponent();

    if (const auto camera = diji::SceneManager::GetInstance().GetMainCamera())
        camera->GetComponent<diji::Camera>()->SetFollow(GetOwner());
}

void superMarioBros::Selector::SetFramePosition(const int frameX, const int frameY)
{
    m_SpriteRenderComp->SetStartingFrame(frameX, frameY);
    m_SpriteRenderComp->UpdateFrame();
    m_CurrentFramePos.x = frameX;
    m_CurrentFramePos.y = frameY;

    m_TemplateSpriteRenderComp->SetStartingFrame(frameX, frameY);
    m_TemplateSpriteRenderComp->UpdateFrame();
}

void superMarioBros::Selector::ActivateBackgroundTexture() const
{
    m_TextureGO->SetActive(true);
}

void superMarioBros::Selector::DeactivateBackgroundTexture() const
{
    m_TextureGO->SetActive(false);
}

void superMarioBros::Selector::TryPlaceItem()
{
    const sf::Vector2f& pos = m_TransformCompPtr->GetWorldPosition();
    const char itemChar = m_AtlasToPosMap[m_CurrentFramePos];
    if (itemChar == '\0')
        return;
    
    const int row = static_cast<int>(pos.y - 25.f) / 50;
    const int col = static_cast<int>(pos.x - 25.f) / 50;
    if (itemChar == '0')
    {
        if (m_PlacedItemsMap[{.row= row, .col= col }])
        {
            m_PlacedItemsMap[{.row= row, .col= col }]->Destroy();
            m_PlacedItemsMap.erase({.row= row, .col= col });
        }
    }
    
    const int cols = LevelEditorManager::GetInstance().SetCharAtPosition(col, row, itemChar);
    if (const auto& placedObject = GameManager::GetInstance().PlaceNewItem(col, row, itemChar))
        m_PlacedItemsMap[{.row= row, .col= col }] = placedObject;
    
    m_BackgroundHandlerRef->TempReload(cols, LevelEditorManager::GetInstance().GetLevelInfo()); //todo: fix this  function
}

void superMarioBros::Selector::TryHoldPlaceItem()
{
    m_IsHolding = !m_IsHolding;
    if (m_IsHolding)
    {
        m_StartingPos = m_TransformCompPtr->GetWorldPosition();
        m_StartingRow = static_cast<int>(m_StartingPos.y - 25.f) / 50;
        m_StartingCol = static_cast<int>(m_StartingPos.x - 25.f) / 50;
        m_PreviewItemsMap[{ .row= m_StartingRow, .col= m_StartingCol }]= diji::SceneManager::GetInstance().SpawnGameObject("X_PreviewTexture", m_ItemTemplateUPtr.get(), m_StartingPos);
        return;
    }

    const char itemChar = m_AtlasToPosMap[m_CurrentFramePos];
    int cols = 0;

    for (auto& [gridPos, preview] : m_PreviewItemsMap)
    {
        if (m_PlacedItemsMap[{.row= gridPos.row, .col= gridPos.col }])
        {
            m_PlacedItemsMap[{.row= gridPos.row, .col= gridPos.col }]->Destroy();
            m_PlacedItemsMap.erase({.row= gridPos.row, .col= gridPos.col });
        }
        
        cols = LevelEditorManager::GetInstance().SetCharAtPosition(gridPos.col, gridPos.row, itemChar);
        if (const auto& placedObject = GameManager::GetInstance().PlaceNewItem(gridPos.col, gridPos.row, itemChar))
            m_PlacedItemsMap[{.row= gridPos.row, .col= gridPos.col }] = placedObject;
        preview->Destroy();
    }

    m_PreviewItemsMap.clear();
    m_BackgroundHandlerRef->TempReload(cols, LevelEditorManager::GetInstance().GetLevelInfo());
}

void superMarioBros::Selector::UpdatePreviewItems()
{
    if (m_IsHolding == false)
        return;
    
    const sf::Vector2f currentPos = m_TransformCompPtr->GetWorldPosition();

    const int currentRow = static_cast<int>(currentPos.y - 25.f) / 50;
    const int currentCol = static_cast<int>(currentPos.x - 25.f) / 50;

    const int minRow = std::min(m_StartingRow, currentRow);
    const int maxRow = std::max(m_StartingRow, currentRow);
    const int minCol = std::min(m_StartingCol, currentCol);
    const int maxCol = std::max(m_StartingCol, currentCol);

    for (auto it = m_PreviewItemsMap.begin(); it != m_PreviewItemsMap.end();)
    {
        const auto& [row, col] = it->first;
        const bool inside = row >= minRow && row <= maxRow && col >= minCol && col <= maxCol;
        if (inside == false)
        {
            it->second->Destroy();
            it = m_PreviewItemsMap.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (int row = minRow; row <= maxRow; ++row)
    {
        for (int col = minCol; col <= maxCol; ++col)
        {
            const GridPos cell{ .row= row, .col= col};
            if (m_PreviewItemsMap.contains(cell))
                continue;

            sf::Vector2f worldPos{ col * 50.f + 25.f, row * 50.f + 25.f };
            m_PreviewItemsMap[cell]= diji::SceneManager::GetInstance().SpawnGameObject("X_PreviewTexture", m_ItemTemplateUPtr.get(), worldPos);
        }
    }

    for (const auto val : m_PreviewItemsMap | std::views::values)
    {
        const auto& sprite = val->GetComponent<diji::SpriteRenderComponent>();
        sprite->SetStartingFrame(m_CurrentFramePos.x, m_CurrentFramePos.y);
        sprite->UpdateFrame();
    }
}

void superMarioBros::Selector::EnablePreview() const
{
    m_TextureGO->SetActive(true);
}

void superMarioBros::Selector::DisablePreview() const
{
    m_TextureGO->SetActive(false);
}

void superMarioBros::Selector::CreateBackgroundTexture()
{
    auto texture = std::make_unique<diji::GameObject>();
    const auto& sprite = texture->AddComponent<diji::SpriteRenderComponent>("graphics/tiles_sheet_selection.png");
    sprite->SetFrameSize(sf::Vector2i{ 50, 50 });
    sprite->SetTotalAnimationFrames(1);
    sprite->Pause();

    m_TextureGO = diji::SceneManager::GetInstance().SpawnGameObject("X_SelectorTexture", std::move(texture), sf::Vector2f{ 0.f, 0.f });
    m_TextureGO->AttachToObject(GetOwner(), false);

    m_SpriteRenderComp = m_TextureGO->GetComponent<diji::SpriteRenderComponent>();
    m_TextureGO->SetActive(false);
}
