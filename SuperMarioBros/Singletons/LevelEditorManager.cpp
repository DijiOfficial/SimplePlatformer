#include "LevelEditorManager.h"

#include "GameManager.h"
#include "../Components/Backgrounds/CustomBackgroundRenderer.h"
#include "../Components/Player/PlayerCharacter.h"
#include "../Core/GameLoader.h"
#include "../Helpers/WorldBuilder.h"
#include "Engine/Collision/PhysicsWorld.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"

#include <fstream>
#include <ranges>

void superMarioBros::LevelEditorManager::Init()
{
    const LevelDataFile levelData = LevelData::Load();

    m_LevelNameToFileUMap.clear();

    for (const auto& [mapName, mapInfo] : levelData.maps)
        m_LevelNameToFileUMap[mapName] = mapInfo.file;

    GameManager::GetInstance().CreateEmptyWorld();
    WorldBuilder::Init();
}

void superMarioBros::LevelEditorManager::LoadLevel(const std::string& levelName)
{
    GameManager::GetInstance().LoadLevel(m_LevelNameToFileUMap[levelName]);
    m_CurrentLevelName = levelName;

    const LevelDataFile levelData = LevelData::Load();
    const auto it = levelData.maps.find(levelName);
    m_CurrentMapInfo = it != levelData.maps.end() ? it->second : MapInfo{};
    m_PendingWarp.reset();
    GameManager::ApplyBackground(m_CurrentMapInfo.background);
    OnMapInfoChangedEvent.Broadcast();
}

void superMarioBros::LevelEditorManager::LoadNewLevel()
{
    GameManager::GetInstance().EmptyLevel();
    m_LevelInfo = std::vector<char>();
    m_CurrentLevelName = "New Level";
    m_LevelWidth = -1;
    m_CurrentMapInfo = MapInfo{};
    m_PendingWarp.reset();
    GameManager::ApplyBackground(m_CurrentMapInfo.background);
    OnLevelLoadedEvent.Broadcast();
    OnMapInfoChangedEvent.Broadcast();
}

void superMarioBros::LevelEditorManager::SaveNewMap(const std::string& levelName, const std::string& levelPath)
{
    m_LevelNameToFileUMap[levelName] = levelPath;
    m_CurrentMapInfo.file = levelPath;

    LevelData::SaveMapInfo(levelName, m_CurrentMapInfo);

    SaveLevelInfo(MAPS_PATH + levelPath);
}

void superMarioBros::LevelEditorManager::SaveMap()
{
    SaveNewMap(m_CurrentLevelName, m_LevelNameToFileUMap.contains(m_CurrentLevelName) ? m_LevelNameToFileUMap[m_CurrentLevelName] : m_CurrentLevelName); // todo: add .txt to failstate
}

std::vector<std::string> superMarioBros::LevelEditorManager::GetLevelNames() const
{
    std::vector<std::string> levelNames;
    for (const auto& key : m_LevelNameToFileUMap | std::views::keys)
    {
        levelNames.push_back(key);
    }
    
    return levelNames;
}

std::string superMarioBros::LevelEditorManager::GetMissingSaveRequirement() const
{
    if (m_CurrentMapInfo.isBonusArea)
    {
        if (m_CurrentMapInfo.arrival.has_value() == false)
            return "Place the bonus area arrival before saving";

        if (m_CurrentMapInfo.exit.has_value() == false)
            return "Place the bonus area exit before saving";

        return {};
    }

    if (m_CurrentMapInfo.start.has_value() == false)
        return "Place the start position before saving";

    return {};
}

void superMarioBros::LevelEditorManager::SetStartCell(const sf::Vector2i& cell)
{
    m_CurrentMapInfo.start = cell;
    OnMapInfoChangedEvent.Broadcast();
}

void superMarioBros::LevelEditorManager::SetBonusAreaArrival(const WarpArrival& arrival)
{
    m_CurrentMapInfo.arrival = arrival;
    OnMapInfoChangedEvent.Broadcast();
}

void superMarioBros::LevelEditorManager::SetBonusAreaExit(const WarpEntrance& exit)
{
    m_CurrentMapInfo.exit = exit;
    OnMapInfoChangedEvent.Broadcast();
}

void superMarioBros::LevelEditorManager::BeginWarp(const WarpEntrance& entrance)
{
    m_PendingWarp = WarpInfo{ .entrance = entrance };  // NOLINT(clang-diagnostic-missing-designated-field-initializers) information is purposely split
    OnMapInfoChangedEvent.Broadcast();
}

void superMarioBros::LevelEditorManager::SetPendingWarpTarget(const std::string& targetMap)
{
    if (m_PendingWarp.has_value() == false)
        return;

    m_PendingWarp->targetMap = targetMap;
    OnWarpTargetChosenEvent.Broadcast();
}

void superMarioBros::LevelEditorManager::CompleteWarp(const WarpArrival& returnPoint)
{
    if (IsWaitingForWarpReturn() == false)
        return;

    if (m_PendingWarp.has_value() == false)
        throw std::runtime_error("No pending warp to complete.");
    
    m_PendingWarp->returnPoint = returnPoint;
    m_CurrentMapInfo.warps.push_back(m_PendingWarp.value());
    m_PendingWarp.reset();
    OnMapInfoChangedEvent.Broadcast();
}

void superMarioBros::LevelEditorManager::CancelPendingWarp()
{
    if (m_PendingWarp.has_value() == false)
        return;

    m_PendingWarp.reset();
    OnMapInfoChangedEvent.Broadcast();
}

void superMarioBros::LevelEditorManager::SetBackground(const std::string& background)
{
    m_CurrentMapInfo.background = background;
    GameManager::ApplyBackground(background);
}

void superMarioBros::LevelEditorManager::StartPlayTest()
{
    if (m_IsPlayTesting || m_CurrentMapInfo.start.has_value() == false)
        return;

    m_IsPlayTesting = true;
    GameManager::GetInstance().PrepareLevelTest(m_CurrentMapInfo.start.value());
    OnPlayTestToggledEvent.Broadcast(true);
    ReloadWorld();
}

void superMarioBros::LevelEditorManager::StopPlayTest()
{
    if (m_IsPlayTesting == false) return;

    if (const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar"))
        m_PlayTestExitPosition = player->GetRootComponent()->GetWorldPosition();

    m_IsPlayTesting = false;
    OnPlayTestToggledEvent.Broadcast(false);
    ReloadWorld();
}

void superMarioBros::LevelEditorManager::RestartPlayTest()
{
    if (m_IsPlayTesting == false) return;

    ReloadWorld();
}

void superMarioBros::LevelEditorManager::ReloadWorld()
{
    if (m_IsReloadingWorld) return;

    m_IsReloadingWorld = true;
    diji::TimerManager::GetInstance().DelayUntilNextTick([this]
    {
        diji::InputManager::GetInstance().ResetCommands();
        diji::SceneManager::GetInstance().GetPhysicsWorld()->DisablePhysics();

        if (const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar"))
            player->Destroy();

        GameManager::GetInstance().DestroyWorld();

        diji::TimerManager::GetInstance().DelayUntilNextTick([this]
        {
            m_IsReloadingWorld = false;
            SpawnPlayer();
        });
    });
}

void superMarioBros::LevelEditorManager::SpawnPlayer() const
{
    const auto& sceneManager = diji::SceneManager::GetInstance();

    auto* cameraComp = sceneManager.GetMainCamera()->GetComponent<diji::Camera>();
    cameraComp->SetLevelBoundaries(WorldBuilder::WorldSettings::ARENA_SIZE);

    auto playerUPtr = std::make_unique<diji::GameObject>();
    SceneLoader::AddPlayerComponents(playerUPtr.get());
    diji::GameObject* player = sceneManager.SpawnGameObject("X_PlayerChar", std::move(playerUPtr), sf::Vector2f{ 200.0f, 0.0f });

    GameManager::GetInstance().BuildEditorWorld(m_LevelInfo);

    auto* backgroundRendererComp = sceneManager.GetGameObject("A_StaticBackground")->GetComponent<CustomBackgroundRenderer>();

    if (m_IsPlayTesting == false)
    {
        player->SetActive(false);

        const diji::GameObject* selector = sceneManager.GetGameObject("X_SelectorPlayer");
        cameraComp->SetFollow(selector);
        backgroundRendererComp->SetFollowTransform(selector->GetRootComponent());
        SceneLoader::BindLevelEditorCommands();
        return;
    }

    auto* playerCharacterComp = player->GetComponent<PlayerCharacter>();
    playerCharacterComp->LinkLevelObjects();
    playerCharacterComp->OnCastleReachedEvent.AddListener([]
    {
        GetInstance().StopPlayTest();
    });

    backgroundRendererComp->SetFollowTransform(player->GetRootComponent());
    sceneManager.GetPhysicsWorld()->RefreshStaticColliders();
    sceneManager.GetPhysicsWorld()->EnablePhysics();
    sceneManager.GetPhysicsWorld()->SetGravity(WorldBuilder::WorldSettings::GRAVITY);
    SceneLoader::BindPlayerCommands(player);
    SceneLoader::BindPlayTestCommands();
}

bool superMarioBros::LevelEditorManager::TryRemoveMarkerAt(const sf::Vector2i& cell)
{
    bool isRemoved = false;

    if (m_CurrentMapInfo.start.has_value() && m_CurrentMapInfo.start.value() == cell)
    {
        m_CurrentMapInfo.start.reset();
        isRemoved = true;
    }

    if (m_CurrentMapInfo.arrival.has_value() && m_CurrentMapInfo.arrival->cell == cell)
    {
        m_CurrentMapInfo.arrival.reset();
        isRemoved = true;
    }

    if (m_CurrentMapInfo.exit.has_value() && m_CurrentMapInfo.exit->cell == cell)
    {
        m_CurrentMapInfo.exit.reset();
        isRemoved = true;
    }

    if (m_PendingWarp.has_value() && m_PendingWarp->entrance.cell == cell)
    {
        m_PendingWarp.reset();
        isRemoved = true;
    }

    const auto removedWarpCount = std::erase_if(m_CurrentMapInfo.warps, [&cell](const WarpInfo& warp)
    {
        return warp.entrance.cell == cell || warp.returnPoint.cell == cell;
    });

    if (isRemoved == false && removedWarpCount == 0)
        return false;

    OnMapInfoChangedEvent.Broadcast();
    return true;
}

std::string superMarioBros::LevelEditorManager::CreateBonusMap()
{
    std::string mapName;
    for (char letter = 'A'; letter <= 'Z'; ++letter)
    {
        std::string candidate = std::string{ "BonusArea" } + letter;
        if (m_LevelNameToFileUMap.contains(candidate) == false)
        {
            mapName = std::move(candidate);
            break;
        }
    }

    if (mapName.empty())
        throw std::runtime_error("No free bonus area name left.");

    const MapInfo bonusMapInfo{ .file = mapName + ".txt", .isBonusArea = true, .background = "black" };  // NOLINT(clang-diagnostic-missing-designated-field-initializers) information is purposely split
    LevelData::SaveMapInfo(mapName, bonusMapInfo);

    std::ofstream file(MAPS_PATH + bonusMapInfo.file);
    if (file.is_open() == false)
        throw std::runtime_error("Could not create level file: " + bonusMapInfo.file);

    m_LevelNameToFileUMap[mapName] = bonusMapInfo.file;

    return mapName;
}

int superMarioBros::LevelEditorManager::SetCharAtPosition(const int x, const int y, const char value)
{
    if (x < 0)
        return -1;

    if (y < 0 || y >= WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT)
        return -1;

    if (x >= m_LevelWidth)
    {
        const int newWidth = x + 1;

        std::vector newLevel(static_cast<size_t>(newWidth) * WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT, '0');
        for (int row = 0; row < WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT; ++row)
        {
            for (int col = 0; col < m_LevelWidth; ++col)
            {
                newLevel[row * newWidth + col] = m_LevelInfo[row * m_LevelWidth + col];
            }
        }

        m_LevelInfo = std::move(newLevel);
        m_LevelWidth = newWidth;
    }

    m_LevelInfo[y * m_LevelWidth + x] = value;

    return m_LevelWidth;
}

char superMarioBros::LevelEditorManager::GetLevelInfoAtPos(const int x, const int y) const
{
    if (x < 0 || y < 0 || y >= WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT)
        return '0';

    const int cols = static_cast<int>(m_LevelInfo.size()) / WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT;

    if (x >= cols)
        return '0';

    return m_LevelInfo[y * cols + x];
}

void superMarioBros::LevelEditorManager::SaveLevelInfo(const std::string& filepath) const
{
    if (m_LevelInfo.size() % WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT != 0)
        throw std::runtime_error("Level data is not divisible by 13.");

    constexpr int rows = WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT;
    const int cols = static_cast<int>(m_LevelInfo.size()) / rows;
    
    std::ofstream file(filepath);

    if (file.is_open() == false)
        throw std::runtime_error("Could not save level file: " + filepath);

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            const int index = row * cols + col;
            file << m_LevelInfo[index];
        }

        file << '\n';
    }

    file.close();
}

void superMarioBros::LevelEditorManager::RecalculateLevelWidth()
{
    if (m_LevelInfo.empty())
    {
        m_LevelWidth = 0;
        return;
    }

    const int rowWidth = static_cast<int>(m_LevelInfo.size()) / WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT;

    int maxWidth = 0;

    for (int y = 0; y < WorldBuilder::WorldSettings::MAX_LEVEL_HEIGHT; ++y)
    {
        const int rowStart = y * rowWidth;
        if (m_LevelInfo[rowStart + rowWidth] != '0')
        {
            m_LevelWidth = rowWidth;
            return;
        }

        int lastNonZero = -1;

        for (int x = 0; x < rowWidth; ++x)
        {
            if (m_LevelInfo[rowStart + x] != '0')
                lastNonZero = x;
        }

        if (lastNonZero != -1)
            maxWidth = std::max(maxWidth, lastNonZero + 1);
    }

    m_LevelWidth = maxWidth;
}
