#include "Selector.h"

#include <ranges>

#include "SelectorControls.h"
#include "../../Helpers/WorldBuilder.h"
#include "../../Singletons/GameManager.h"
#include "../../Singletons/LevelEditorManager.h"
#include "../Backgrounds/BackgroundHandler.h"
#include "../Enemies/BaseEnemy.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::Selector::Init()
{
    LevelEditorManager::GetInstance().Init();

    const sf::Vector2f startPos = LevelEditorManager::GetInstance().m_SelectorPosition;
    if (diji::Helpers::AreVectorsEqual(startPos, sf::Vector2f{ 0.0f, 0.0f }) == false)
    {
        GetOwner()->SetObjectPosition(startPos);
    }

    CreateBackgroundTexture();
    
    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_TemplateSpriteRenderComp = m_ItemTemplateUPtr->AddComponent<diji::SpriteRenderComponent>(TILES_SELECTION_SHEET, sf::Vector2i{ 50, 50 }, 1, 1.0f);

    m_EnemyTemplateUPtr = std::make_unique<diji::GameObject>();
    m_EnemyTemplateSpriteRenderComp = m_EnemyTemplateUPtr->AddComponent<diji::SpriteRenderComponent>(ENEMIES_SELECTION_SHEET, sf::Vector2i{ 50, 50 }, 1, 1.0f);

    LevelEditorManager::GetInstance().OnLevelLoadedEvent.AddListener(this, &Selector::CreateAllSpecialBlocks);
    LevelEditorManager::GetInstance().OnMapInfoChangedEvent.AddListener(this, &Selector::RefreshMarkers);
    LevelEditorManager::GetInstance().OnWarpTargetChosenEvent.AddListener(this, &Selector::SelectArrivalTool);
    LevelEditorManager::GetInstance().OnPlayTestToggledEvent.AddListener(this, &Selector::OnPlayTestToggled);
    SetRenderLayer(3);
}

void superMarioBros::Selector::Start()
{
    m_TransformCompPtr = GetOwner()->GetRootComponent();
    m_SelectorControlsRef = GetOwner()->GetComponent<SelectorControls>();

    if (const auto camera = diji::SceneManager::GetInstance().GetMainCamera())
        camera->GetComponent<diji::Camera>()->SetFollow(GetOwner());
}

void superMarioBros::Selector::SetFramePosition(const int frameX, const int frameY)
{
    SetSelectedSheet(false);
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
    const int row = static_cast<int>(pos.y - 25.0f) / 50;
    const int col = static_cast<int>(pos.x - 25.0f) / 50;

    if (IsMarkerToolSelected())
    {
        TryPlaceMarker(sf::Vector2i{ col, row });
        return;
    }

    const char itemChar = GetSelectedItemChar();
    if (itemChar == '\0')
        return;

    const GridPos gridPos{ .row= row, .col= col };
    if (itemChar == 'r')
    {
        TryPlaceCheckPoint(gridPos);
        return;
    }

    if (m_IsEnemySelected)
    {
        PlaceEnemy(itemChar, gridPos);
        return;
    }

    if (itemChar == '0')
    {
        if (LevelEditorManager::GetInstance().TryRemoveMarkerAt(sf::Vector2i{ col, row }))
            return;

        TryDeletePlacedItem(gridPos);
    }
    
    if (HandleSpecialItems(itemChar, gridPos))
        return;

    CreateItemAtGridPos(itemChar, gridPos);
}

void superMarioBros::Selector::TryHoldPlaceItem()
{
    if (m_IsHolding == false && IsMarkerToolSelected())
    {
        LevelEditorManager::GetInstance().CancelPendingWarp();
        return;
    }

    if (m_IsHolding == false && GetSelectedItemChar() == 'r')
        return;

    m_IsHolding = m_IsHolding == false;
    if (m_IsHolding)
    {
        m_StartingPos = m_TransformCompPtr->GetWorldPosition();
        m_StartingRow = static_cast<int>(m_StartingPos.y - 25.0f) / 50;
        m_StartingCol = static_cast<int>(m_StartingPos.x - 25.0f) / 50;
        m_PreviewItemsMap[{ .row= m_StartingRow, .col= m_StartingCol }]= diji::SceneManager::GetInstance().SpawnGameObject("X_PreviewTexture", GetPreviewTemplate(), m_StartingPos + GetPreviewOffset());
        return;
    }

    const char itemChar = GetSelectedItemChar();
    int cols = 0;
    std::vector<GridPos> piranhaPlantCells;

    for (auto& [gridPos, preview] : m_PreviewItemsMap)
    {
        preview->Destroy();

        if (itemChar == 's')
        {
            piranhaPlantCells.push_back(gridPos);
            continue;
        }

        TryDeletePlacedItem(gridPos);

        cols = LevelEditorManager::GetInstance().SetCharAtPosition(gridPos.col, gridPos.row, itemChar);
        for (const diji::GameObject* placedObject : GameManager::GetInstance().PlaceNewItem(gridPos.col, gridPos.row, itemChar))
        {
            m_PlacedItemsMap[gridPos].insert(placedObject);
            if (m_IsEnemySelected)
                PrepareEnemyForEditing(placedObject);
        }
    }

    m_PreviewItemsMap.clear();

    if (itemChar == 's')
    {
        std::unordered_set<GridPos, GridPosHasher> pipeTops;
        for (const GridPos& cell : piranhaPlantCells)
        {
            const std::optional<GridPos> pipeTop = GetPipeTopLeft(cell);
            if (pipeTop.has_value() && pipeTops.insert(pipeTop.value()).second)
                TryPlacePiranhaPlant(pipeTop.value());
        }
        return;
    }

    m_BackgroundHandlerRef->TempReload(cols, LevelEditorManager::GetInstance().GetLevelInfo());
}

void superMarioBros::Selector::UpdatePreviewItems()
{
    if (m_IsHolding == false)
        return;
    
    const sf::Vector2f currentPos = m_TransformCompPtr->GetWorldPosition();

    const int currentRow = static_cast<int>(currentPos.y - 25.0f) / 50;
    const int currentCol = static_cast<int>(currentPos.x - 25.0f) / 50;

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

            sf::Vector2f worldPos{ col * 50.0f + 25.0f, row * 50.0f + 25.0f };
            m_PreviewItemsMap[cell]= diji::SceneManager::GetInstance().SpawnGameObject("X_PreviewTexture", GetPreviewTemplate(), worldPos + GetPreviewOffset());
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

sf::Vector2i superMarioBros::Selector::GetGridPosFromChar(const char itemChar) const
{
    for (const auto [pos, item] : m_GridToCharMap)
    {
        if (item == itemChar)
            return sf::Vector2i{ pos.x, pos.y };
    }
    return sf::Vector2i{ -1, -1 };
}

void superMarioBros::Selector::SelectEnemy(const sf::Vector2i& frame)
{
    const auto it = m_EnemyGridToCharMap.find(frame);
    if (it == m_EnemyGridToCharMap.end())
        throw std::runtime_error("Invalid enemy selected at grid position: " + std::to_string(frame.x) + ", " + std::to_string(frame.y));

    SetSelectedSheet(true);
    m_SelectedEnemyChar = it->second;
    m_CurrentFramePos = frame;

    m_SpriteRenderComp->SetStartingFrame(frame.x, frame.y);
    m_SpriteRenderComp->UpdateFrame();
    m_EnemyTemplateSpriteRenderComp->SetStartingFrame(frame.x, frame.y);
    m_EnemyTemplateSpriteRenderComp->UpdateFrame();

    m_TextureGO->GetRootComponent()->SetWorldPosition(m_TransformCompPtr->GetWorldPosition() + GetPreviewOffset());
}

void superMarioBros::Selector::SelectItemFromChar(const char itemChar)
{
    for (const auto& [frame, enemyChar] : m_EnemyGridToCharMap)
    {
        if (enemyChar == itemChar)
        {
            SelectEnemy(frame);
            return;
        }
    }

    const sf::Vector2i frame = GetGridPosFromChar(itemChar);
    SetFramePosition(frame.x, frame.y);
}

char superMarioBros::Selector::GetSelectedItemChar() const
{
    if (m_IsEnemySelected)
        return m_SelectedEnemyChar;

    const auto it = m_GridToCharMap.find(m_CurrentFramePos);
    return it != m_GridToCharMap.end() ? it->second : '\0';
}

const diji::GameObject* superMarioBros::Selector::GetPreviewTemplate() const
{
    return m_IsEnemySelected ? m_EnemyTemplateUPtr.get() : m_ItemTemplateUPtr.get();
}

sf::Vector2f superMarioBros::Selector::GetPreviewOffset() const
{
    const char itemChar = GetSelectedItemChar();
    if (itemChar == 'h' || itemChar == 'p')
        return ENEMY_OFFSET;

    return sf::Vector2f{ 0.0f, 0.0f };
}

void superMarioBros::Selector::SetSelectedSheet(const bool isEnemySheet)
{
    if (m_IsEnemySelected == isEnemySheet) return;

    m_IsEnemySelected = isEnemySheet;
    m_SpriteRenderComp->SetTexture(isEnemySheet ? ENEMIES_SELECTION_SHEET : TILES_SELECTION_SHEET);

    if (isEnemySheet == false)
        m_TextureGO->GetRootComponent()->SetWorldPosition(m_TransformCompPtr->GetWorldPosition());
}

void superMarioBros::Selector::PrepareEnemyForEditing(const diji::GameObject* enemy)
{
    auto* enemyComp = enemy->GetComponent<BaseEnemy>();
    if (enemyComp == nullptr) return;

    enemyComp->SetActive(true);
    enemyComp->Pause();
}

void superMarioBros::Selector::CreateBackgroundTexture()
{
    auto texture = std::make_unique<diji::GameObject>();
    const auto& sprite = texture->AddComponent<diji::SpriteRenderComponent>(TILES_SELECTION_SHEET);
    sprite->SetFrameSize(sf::Vector2i{ 50, 50 });
    sprite->SetTotalAnimationFrames(1);
    sprite->Pause();

    m_TextureGO = diji::SceneManager::GetInstance().SpawnGameObject("X_SelectorTexture", std::move(texture), sf::Vector2f{ 0.0f, 0.0f });
    m_TextureGO->AttachToObject(GetOwner(), false);

    m_SpriteRenderComp = m_TextureGO->GetComponent<diji::SpriteRenderComponent>();
    m_TextureGO->SetActive(false);
    m_TextureGO->SetRenderLayer(2);
}

bool superMarioBros::Selector::HandleSpecialItems(const char item, const GridPos& gridPos)
{
    const char currentItem = LevelEditorManager::GetInstance().GetLevelInfoAtPos(gridPos.col, gridPos.row);

    if (item == 'x')
    {
        TryDeletePlacedItem(gridPos);
        CreateItemAtGridPos('x', gridPos);
        HandleCreatingSpecialBlocks('x', gridPos);
        return true;
    }

    if (m_PlacedItemsMap[gridPos].empty())
        return false;
    
    const auto ReplaceSpecialItem = [&](const char target)
    {
        TryDeletePlacedItem(gridPos);
        CreateItemAtGridPos(target, gridPos);
        HandleCreatingSpecialBlocks(target, gridPos);
    };

    const auto it = m_ReplacementsMap.find(item);
    if (it == m_ReplacementsMap.end())
        return false;
    
    if (currentItem == 'd')
        ReplaceSpecialItem(it->second.first);
    else if (currentItem == 'H')
        ReplaceSpecialItem(it->second.second);
    else
        return false;

    return true;
}

void superMarioBros::Selector::CreateItemAtGridPos(const char itemChar, const GridPos& gridPos)
{
    const int cols = LevelEditorManager::GetInstance().SetCharAtPosition(gridPos.col, gridPos.row, itemChar);
    for (const diji::GameObject* placedObject : GameManager::GetInstance().PlaceNewItem(gridPos.col, gridPos.row, itemChar))
        m_PlacedItemsMap[gridPos].insert(placedObject);

    m_BackgroundHandlerRef->TempReload(cols, LevelEditorManager::GetInstance().GetLevelInfo()); //todo: fix this  function
}

void superMarioBros::Selector::CreateAllSpecialBlocks()
{
    const auto& levelInfo = LevelEditorManager::GetInstance().GetLevelInfo();
    m_PlacedItemsMap = std::unordered_map<GridPos, std::unordered_set<const diji::GameObject*>, GridPosHasher>();

    if (levelInfo.empty())
        return;

    const bool isPlayTesting = LevelEditorManager::GetInstance().IsPlayTesting();
    const int cols = static_cast<int>(levelInfo.size()) / WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT;
    for (int index = 0; index < static_cast<int>(levelInfo.size()); ++index)
    {
        const char item = levelInfo[index];

        if (item == '0')
            continue;

        const int row = index / cols;
        const int col = index % cols;
        const GridPos gridPos = { .row = row, .col = col };

        CreateItemAtGridPos(item, { .row = row, .col = col });
        if (isPlayTesting == false)
            HandleCreatingSpecialBlocks(item, gridPos);
    }

    if (isPlayTesting)
        return;

    for (const auto& placedObjects : m_PlacedItemsMap | std::views::values)
    {
        for (const diji::GameObject* placedObject : placedObjects)
            PrepareEnemyForEditing(placedObject);
    }
}

void superMarioBros::Selector::TryDeletePlacedItem(const GridPos& gridPos)
{
    if (m_PlacedItemsMap[gridPos].empty() == false)
    {
        for (const auto& gameObject : m_PlacedItemsMap[gridPos])
        {
            GameManager::GetInstance().RemoveEnemyCollider(gameObject->GetComponent<diji::Collider>());
            gameObject->Destroy();
        }
        m_PlacedItemsMap.erase(gridPos);
    }
}

void superMarioBros::Selector::HandleCreatingSpecialBlocks(const char itemChar, const GridPos& gridPos)
{
    const auto& gameObject = diji::SceneManager::GetInstance().SpawnGameObject("X_PreviewTexture", m_ItemTemplateUPtr.get(), sf::Vector2f{ gridPos.col * 50 + 25.0f, gridPos.row * 50 + 25.0f });
    gameObject->SetRenderLayer(2);
    const auto& sprite = gameObject->GetComponent<diji::SpriteRenderComponent>();
    const auto& spritePos = m_CharToGridMap[itemChar];
    sprite->SetStartingFrame(spritePos.x, spritePos.y);
    sprite->UpdateFrame();
    m_PlacedItemsMap[gridPos].insert(gameObject);
    GameManager::GetInstance().AttachToWorldObject(gameObject);
}

void superMarioBros::Selector::TryPlaceCheckPoint(const GridPos& gridPos)
{
    if (LevelEditorManager::GetInstance().GetCurrentMapInfo().isBonusArea)
        return;

    RemoveAllCheckPoints();
    TryDeletePlacedItem(gridPos);
    CreateItemAtGridPos('r', gridPos);
    HandleCreatingSpecialBlocks('r', gridPos);
}

void superMarioBros::Selector::RemoveAllCheckPoints()
{
    const auto& levelInfo = LevelEditorManager::GetInstance().GetLevelInfo();
    if (levelInfo.empty())
        return;

    const int cols = static_cast<int>(levelInfo.size()) / WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT;
    std::vector<GridPos> checkPointPositions;
    for (int index = 0; index < static_cast<int>(levelInfo.size()); ++index)
    {
        if (levelInfo[index] == 'r')
            checkPointPositions.push_back(GridPos{ .row = index / cols, .col = index % cols });
    }

    for (const GridPos& checkPointPos : checkPointPositions)
    {
        TryDeletePlacedItem(checkPointPos);
        CreateItemAtGridPos('0', checkPointPos);
    }
}

void superMarioBros::Selector::PlaceEnemy(const char enemyChar, const GridPos& gridPos)
{
    if (enemyChar == 's')
    {
        if (const std::optional<GridPos> pipeTop = GetPipeTopLeft(gridPos))
            TryPlacePiranhaPlant(pipeTop.value());
        return;
    }

    TryDeletePlacedItem(gridPos);
    CreateItemAtGridPos(enemyChar, gridPos);

    for (const diji::GameObject* placedObject : m_PlacedItemsMap[gridPos])
        PrepareEnemyForEditing(placedObject);
}

void superMarioBros::Selector::TryPlacePiranhaPlant(const GridPos& gridPos)
{
    TryDeletePlacedItem(gridPos);
    CreateItemAtGridPos('s', gridPos);

    for (const diji::GameObject* placedObject : m_PlacedItemsMap[gridPos])
        PrepareEnemyForEditing(placedObject);
}

std::optional<superMarioBros::Selector::GridPos> superMarioBros::Selector::GetPipeTopLeft(const GridPos& gridPos) const
{
    const LevelEditorManager& levelEditorManager = LevelEditorManager::GetInstance();
    int col = gridPos.col;
    int row = gridPos.row;
    char tile = levelEditorManager.GetLevelInfoAtPos(col, row);

    if (tile == '4' || tile == '6')
    {
        --col;
        tile = levelEditorManager.GetLevelInfoAtPos(col, row);
    }

    while (tile == '5' && row > 0)
    {
        --row;
        tile = levelEditorManager.GetLevelInfoAtPos(col, row);
    }

    if (tile == '3' || tile == 's')
        return GridPos{ .row = row, .col = col };

    return std::nullopt;
}

bool superMarioBros::Selector::IsMarkerToolSelected() const
{
    if (m_IsEnemySelected)
        return false;

    return m_CurrentFramePos == START_TOOL_FRAME || m_CurrentFramePos == WARP_TOOL_FRAME || m_CurrentFramePos == ARRIVAL_TOOL_FRAME;
}

void superMarioBros::Selector::TryPlaceMarker(const sf::Vector2i& cell) const
{
    LevelEditorManager& levelEditorManager = LevelEditorManager::GetInstance();
    const char tile = levelEditorManager.GetLevelInfoAtPos(cell.x, cell.y);

    if (m_CurrentFramePos == START_TOOL_FRAME)
    {
        if (levelEditorManager.GetCurrentMapInfo().isBonusArea)
            return;

        levelEditorManager.SetStartCell(cell);
        return;
    }

    if (m_CurrentFramePos == WARP_TOOL_FRAME)
    {
        TryPlaceWarpEntrance(cell, tile);
        return;
    }

    PlaceArrival(cell, tile);
}

void superMarioBros::Selector::TryPlaceWarpEntrance(const sf::Vector2i& cell, const char tile) const
{
    WarpEntrance entrance;
    switch (tile)
    {
    case '3':
        entrance = WarpEntrance{ .cell = cell, .direction = diji::Directions::Down };
        break;
    case '4':
        entrance = WarpEntrance{ .cell = sf::Vector2i{ cell.x - 1, cell.y }, .direction = diji::Directions::Down };
        break;
    case '7':
        entrance = WarpEntrance{ .cell = cell, .direction = diji::Directions::Right };
        break;
    case 'a':
        entrance = WarpEntrance{ .cell = sf::Vector2i{ cell.x, cell.y - 1 }, .direction = diji::Directions::Right };
        break;
    default:
        return;
    }

    LevelEditorManager& levelEditorManager = LevelEditorManager::GetInstance();
    if (levelEditorManager.GetCurrentMapInfo().isBonusArea)
    {
        levelEditorManager.SetBonusAreaExit(entrance);
        return;
    }

    levelEditorManager.BeginWarp(entrance);
    m_SelectorControlsRef->OpenWarpTargetMenu();
}

void superMarioBros::Selector::PlaceArrival(const sf::Vector2i& cell, const char tile) const
{
    WarpArrival arrival{ .cell = cell, .exitType = WarpExitType::Drop };
    if (tile == '3')
        arrival = WarpArrival{ .cell = cell, .exitType = WarpExitType::RiseFromPipe };
    else if (tile == '4')
        arrival = WarpArrival{ .cell = sf::Vector2i{ cell.x - 1, cell.y }, .exitType = WarpExitType::RiseFromPipe };

    LevelEditorManager& levelEditorManager = LevelEditorManager::GetInstance();
    if (levelEditorManager.GetCurrentMapInfo().isBonusArea)
    {
        levelEditorManager.SetBonusAreaArrival(arrival);
        return;
    }

    levelEditorManager.CompleteWarp(arrival);
}

void superMarioBros::Selector::RefreshMarkers()
{
    for (const diji::GameObject* marker : m_MarkerObjects)
        marker->Destroy();

    m_MarkerObjects.clear();

    const LevelEditorManager& levelEditorManager = LevelEditorManager::GetInstance();
    if (levelEditorManager.IsPlayTesting())
        return;

    const MapInfo& mapInfo = levelEditorManager.GetCurrentMapInfo();

    if (mapInfo.start.has_value())
        SpawnMarker(mapInfo.start.value(), START_TOOL_FRAME);

    if (mapInfo.exit.has_value())
        SpawnMarker(mapInfo.exit->cell, WARP_TOOL_FRAME);

    if (mapInfo.arrival.has_value())
        SpawnMarker(mapInfo.arrival->cell, ARRIVAL_TOOL_FRAME);

    for (const WarpInfo& warp : mapInfo.warps)
    {
        SpawnMarker(warp.entrance.cell, WARP_TOOL_FRAME);
        SpawnMarker(warp.returnPoint.cell, ARRIVAL_TOOL_FRAME);
    }

    if (levelEditorManager.GetPendingWarp().has_value())
        SpawnMarker(levelEditorManager.GetPendingWarp()->entrance.cell, WARP_TOOL_FRAME);
}

void superMarioBros::Selector::SpawnMarker(const sf::Vector2i& cell, const sf::Vector2i& frame)
{
    constexpr float tileSize = WorldBuilder::WorldSettings::TILE_SIZE;
    const sf::Vector2f position{ static_cast<float>(cell.x) * tileSize + tileSize * 0.5f, static_cast<float>(cell.y) * tileSize + tileSize * 0.5f };

    const auto& marker = diji::SceneManager::GetInstance().SpawnGameObject("X_MarkerTexture", m_ItemTemplateUPtr.get(), position);
    marker->SetRenderLayer(MARKER_RENDER_LAYER);

    const auto& sprite = marker->GetComponent<diji::SpriteRenderComponent>();
    sprite->SetStartingFrame(frame.x, frame.y);
    sprite->UpdateFrame();

    m_MarkerObjects.push_back(marker);
}

void superMarioBros::Selector::OnPlayTestToggled(bool)
{
    RefreshMarkers();
}

void superMarioBros::Selector::SelectArrivalTool()
{
    SetFramePosition(ARRIVAL_TOOL_FRAME.x, ARRIVAL_TOOL_FRAME.y);
}
