#include "GameManager.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Camera.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "../Core/GameState.h"
#include "../Components/Other/HUD/PointsBehaviour.h"
#include "../Components/Player/PlayerCharacter.h"

#include <format>
#include <fstream>

#include "LevelEditorManager.h"
#include "../Helpers/WorldBuilder.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/JSONLoader.h"
#include "Engine/Singleton/TimerManager.h"

// todo: replace the manual level data parsing with C++26 static reflection once MSVC supports it
namespace
{
    sf::Vector2i ParseCell(const JSONLoader::json& cellData)
    {
        return sf::Vector2i{ cellData.at(0).get<int>(), cellData.at(1).get<int>() };
    }

    diji::Directions ParseDirection(const std::string& direction)
    {
        if (direction == "Left")
            return diji::Directions::Left;
        if (direction == "Right")
            return diji::Directions::Right;
        if (direction == "Up")
            return diji::Directions::Up;
        if (direction == "Down")
            return diji::Directions::Down;

        throw std::runtime_error("Invalid warp direction in level data: " + direction);
    }

    superMarioBros::WarpExitType ParseExitType(const std::string& exitType)
    {
        if (exitType == "Drop")
            return superMarioBros::WarpExitType::Drop;
        if (exitType == "RiseFromPipe")
            return superMarioBros::WarpExitType::RiseFromPipe;

        throw std::runtime_error("Invalid warp exit type in level data: " + exitType);
    }

    superMarioBros::WarpEntrance ParseEntrance(const JSONLoader::json& entranceData)
    {
        return superMarioBros::WarpEntrance{ .cell = ParseCell(entranceData.at("cell")), .direction = ParseDirection(entranceData.at("enter").get<std::string>()) };
    }

    superMarioBros::WarpArrival ParseArrival(const JSONLoader::json& arrivalData)
    {
        return superMarioBros::WarpArrival{ .cell = ParseCell(arrivalData.at("cell")), .exitType = ParseExitType(arrivalData.at("exit").get<std::string>()) };
    }

    superMarioBros::MapInfo ParseMapInfo(const JSONLoader::json& mapData)
    {
        superMarioBros::MapInfo mapInfo;
        mapInfo.file = mapData.at("file").get<std::string>();
        mapInfo.isBonusArea = mapData.value("type", std::string{}) == "Bonus";

        if (mapData.contains("start"))
            mapInfo.start = ParseCell(mapData.at("start"));

        if (mapData.contains("arrival"))
            mapInfo.arrival = ParseArrival(mapData.at("arrival"));

        if (mapData.contains("exit"))
            mapInfo.exit = ParseEntrance(mapData.at("exit"));

        if (mapData.contains("warps"))
        {
            for (const auto& warpData : mapData.at("warps"))
            {
                mapInfo.warps.push_back(superMarioBros::WarpInfo
                {
                    .entrance = ParseEntrance(warpData.at("entrance")),
                    .targetMap = warpData.at("map").get<std::string>(),
                    .returnPoint = ParseArrival(warpData.at("return"))
                });
            }
        }

        return mapInfo;
    }
}

void superMarioBros::GameManager::LoadLevel()
{
    m_IsLevelAlreadyCleared = false;
    m_CurrentPlayerState = m_LastPlayerState == PlayerHealthState::Small ? PlayerHealthState::Small : PlayerHealthState::Big;

    LoadLevelData();

    const bool isTransitionLevel = m_ShouldPlayTransition;
    m_ShouldPlayTransition = false;

    const MapInfo& mapInfo = GetMapInfo(isTransitionLevel ? m_TransitionMapName : GetCurrentLevelEntry().map);
    m_StartPosition = static_cast<sf::Vector2u>(static_cast<sf::Vector2f>(mapInfo.start) * WorldBuilder::WorldSettings::TILE_SIZE);

    ReadLevelInfo(MAPS_PATH + mapInfo.file);
    const diji::GameObject* world = WorldBuilder::CreateWorld(m_LevelInfo, m_Rows, m_Cols);

    OnNewLevelLoadedEvent.Broadcast();

    ResetBonusArea();
    if (isTransitionLevel)
        return;

    for (const WarpInfo& warp : mapInfo.warps)
    {
        WorldBuilder::SpawnWarpEntrance(world, warp.entrance.cell, warp.entrance.direction);
        WorldBuilder::SpawnWarpArrival(world, warp.returnPoint.cell, warp.returnPoint.exitType);
    }

    // todo: only one bonus area per level is supported until the bonus background can hold multiple maps
    if (mapInfo.warps.empty() == false)
        LoadBonusArea(mapInfo.warps.front());
}

void superMarioBros::GameManager::LoadLevel(const std::string& levelFilePath)
{
    if (m_WorldGameObject)
    {
        m_WorldGameObject->Destroy();
        m_WorldGameObject = nullptr;
        m_EnemyColliders = std::unordered_set<diji::Collider*>();
    }

    diji::TimerManager::GetInstance().DelayUntilNextTick([&]
    {
        ReadLevelInfo(std::format("../SuperMarioBros/Resources/levels/{}", levelFilePath));  //todo:  literal string
        CreateEmptyWorld();
        LevelEditorManager::GetInstance().SetLevelInfo(m_LevelInfo);
        OnNewLevelLoadedEvent.Broadcast();
    });
}

void superMarioBros::GameManager::EmptyLevel()
{
    if (m_WorldGameObject)
    {
        m_WorldGameObject->Destroy();
        m_WorldGameObject = nullptr;
        m_EnemyColliders = std::unordered_set<diji::Collider*>();
        CreateEmptyWorld();
    }
    m_Rows = 0;
    m_Cols = 0;
    OnNewLevelLoadedEvent.Broadcast();
}

void superMarioBros::GameManager::SwitchToNextScene()
{
    ClearListeners();
    
    if (m_ShouldPlayTransition)
        diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(superMarioBrosState::TransitionToNextLevel));
    else
        diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(superMarioBrosState::Level));
}

void superMarioBros::GameManager::SetLevelCleared()
{
    if (m_IsLevelAlreadyCleared) return;
    m_IsLevelAlreadyCleared = true;
    
    SavePlayerState();
    ++m_PlayerInfo.currentLevel;
    m_PlayerInfo.checkPointActivated = false;
    m_ShouldPlayTransition = true;

    OnLevelClearedEvent.Broadcast();
    ResetLevel(false);
}

void superMarioBros::GameManager::ResetLevel(const bool playerDied)
{
    m_EnemyColliders = std::unordered_set<diji::Collider*>();

    if (playerDied)
    {
        m_CurrentPlayerState = PlayerHealthState::Small;
        m_LastPlayerState = PlayerHealthState::Small;
    }
    
    ClearListeners();
        
    if (m_PlayerInfo.totalLives == 0)
        diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(superMarioBrosState::GameOver));
    else
        diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(superMarioBrosState::LivesDisplayMenu));

    if (m_PlayerInfo.currentLevel > static_cast<int>(m_Levels.size()))
    {
        // trigger end screen?
        SaveHighScoreToFile();

        diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(superMarioBrosState::StartMenu));
        ResetPlayerInfo();
    }
    
    m_TotalFireballsInLevel = 0;
}

void superMarioBros::GameManager::SwitchCurrentPlayerState()
{
    if (m_CurrentPlayerState == PlayerHealthState::Small)
        m_CurrentPlayerState = PlayerHealthState::Big;
    else
        m_CurrentPlayerState = PlayerHealthState::Small;
}

void superMarioBros::GameManager::SavePlayerState()
{
    const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>();

    if (player->IsSmallMario())
    {
        m_LastPlayerState = PlayerHealthState::Small;
        return;
    }

    if (player->IsFireMario())
    {
        m_LastPlayerState = PlayerHealthState::Fire;
        return;
    }

    m_LastPlayerState = PlayerHealthState::Big;
}

void superMarioBros::GameManager::SpawnPointsText(const sf::Vector2f& position, const std::string& score)
{
    const sf::Vector2f screenPos = diji::SceneManager::GetInstance().GetScreenPosition(position);
    // screenPos.y += static_cast<float>(window::VIEWPORT.y) * 0.5f;
    auto pointsText = std::make_unique<diji::GameObject>();
    pointsText->SetObjectPosition(screenPos);
    pointsText->AddComponent<diji::TextComp>(score, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    pointsText->GetComponent<diji::TextComp>()->GetText().setCharacterSize(18);
    pointsText->AddComponent<diji::Render>();
    pointsText->AddComponent<PointsBehaviour>();

    diji::SceneManager::GetInstance().OverwriteCanvasObject("ZZ_pointsText", std::move(pointsText), screenPos);
}

void superMarioBros::GameManager::AddLife()
{
    ++m_PlayerInfo.totalLives;
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_1-up.wav", false);
}

void superMarioBros::GameManager::ResetPlayerInfo()
{
    m_PlayerInfo.totalLives = 3;
    m_PlayerInfo.totalCoins = 0;
    m_PlayerInfo.totalScore = 0;
    m_PlayerInfo.currentLevel = 1;
    m_PlayerInfo.checkPointActivated = false;
}

int superMarioBros::GameManager::GetHighScoreFromFile() const
{
    std::ifstream file(m_HighScoreFileName);
    if (file.is_open() == false)
        throw std::runtime_error("Could not open high score file: " + m_HighScoreFileName);

    int highScore = 0;
    file >> highScore;
    // if (!file.good())
    //     throw std::runtime_error("Error reading high score from file: " + m_HighScoreFileName);

    file.close();
    return highScore;
}

void superMarioBros::GameManager::SaveHighScoreToFile() const
{
    // I can probably optimize this by reading it at the start of the program and keeping track of the high score until the end of the program avoiding file I/O multiple times
    const int currentHigh = GetHighScoreFromFile();
    if (m_PlayerInfo.totalScore <= currentHigh)
        return;

    std::ofstream file(m_HighScoreFileName, std::ios::trunc);
    if (file.is_open() == false)
        throw std::runtime_error("Could not open high score file: " + m_HighScoreFileName);


    file << m_PlayerInfo.totalScore;
    file.close();
}

void superMarioBros::GameManager::CreateEmptyWorld()
{
    m_WorldGameObject = diji::SceneManager::GetInstance().SpawnGameObject("WorldLevel", std::make_unique<diji::GameObject>(), sf::Vector2f{ 0.0f, 0.0f });
}

const diji::GameObject* superMarioBros::GameManager::PlaceNewItem(const int x, const int y, const char itemID) const
{
    return WorldBuilder::CreateTileObject(m_WorldGameObject, y, x, itemID);
}

void superMarioBros::GameManager::AttachToWorldObject(const diji::GameObject* object) const
{
    object->AttachToObject(m_WorldGameObject, true);
}

void superMarioBros::GameManager::PrepareLoadBonusArea(const diji::Directions direction)
{
    OnBonusAreaTriggeredEvent.Broadcast(direction);
}

void superMarioBros::GameManager::HideCurrentArea()
{
    const auto& sceneManager = diji::SceneManager::GetInstance();

    if (m_IsInBonusArea)
    {
        m_BonusAreaWorldGameObject->SetActive(false);
        sceneManager.GetGameObject("B_BonusAreaBackground")->GetComponent<diji::Render>()->DisableRender();
        return;
    }

    auto* camera = sceneManager.GetMainCamera()->GetComponent<diji::Camera>();
    const float bonusAreaCenterX = (static_cast<float>(BONUS_AREA_COLUMN_OFFSET) + static_cast<float>(m_BonusAreaCols) * 0.5f) * WorldBuilder::WorldSettings::TILE_SIZE;
    camera->ForceCameraPosition(sf::Vector2f{ bonusAreaCenterX, camera->GetCameraView().getCenter().y });

    m_LevelBackgroundColor = diji::Renderer::GetInstance().GetBackgroundColor();
    diji::Renderer::GetInstance().SetBackgroundColor(sf::Color::Black);
    sceneManager.GetGameObject("A_StaticBackground")->GetComponent<diji::Render>()->DisableRender();
    sceneManager.GetGameObject("B_Background")->GetComponent<diji::Render>()->DisableRender();
}

superMarioBros::AreaArrival superMarioBros::GameManager::ShowNextArea()
{
    const auto& sceneManager = diji::SceneManager::GetInstance();
    m_IsInBonusArea = m_IsInBonusArea == false;

    if (m_IsInBonusArea)
    {
        m_BonusAreaWorldGameObject->SetActive(true);
        sceneManager.GetGameObject("B_BonusAreaBackground")->GetComponent<diji::Render>()->EnableRender();
        diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/UndergroundMusic.mp3", true);

        return m_BonusAreaArrival;
    }

    sceneManager.GetMainCamera()->GetComponent<diji::Camera>()->ReleaseForcedCameraPosition();

    diji::Renderer::GetInstance().SetBackgroundColor(m_LevelBackgroundColor);
    sceneManager.GetGameObject("A_StaticBackground")->GetComponent<diji::Render>()->EnableRender();
    sceneManager.GetGameObject("B_Background")->GetComponent<diji::Render>()->EnableRender();
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/LevelMusic.mp3", true);

    return m_BonusAreaReturn;
}

void superMarioBros::GameManager::ResetBonusArea()
{
    m_IsInBonusArea = false;
    m_BonusAreaWorldGameObject = nullptr;
    m_BonusAreaInfo = std::vector<char>();
    m_BonusAreaRows = 0;
    m_BonusAreaCols = 0;
}

void superMarioBros::GameManager::LoadBonusArea(const WarpInfo& warp)
{
    const MapInfo& bonusMapInfo = GetMapInfo(warp.targetMap);

    ReadLevelFile(MAPS_PATH + bonusMapInfo.file, m_BonusAreaInfo, m_BonusAreaRows, m_BonusAreaCols);

    m_BonusAreaWorldGameObject = WorldBuilder::CreateWorld(m_BonusAreaInfo, m_BonusAreaRows, m_BonusAreaCols, BONUS_AREA_COLUMN_OFFSET);
    WorldBuilder::SpawnWarpEntrance(m_BonusAreaWorldGameObject, bonusMapInfo.exit.cell, bonusMapInfo.exit.direction, BONUS_AREA_COLUMN_OFFSET);
    m_BonusAreaWorldGameObject->SetActive(false);

    m_BonusAreaArrival = AreaArrival{ .position = WorldBuilder::GetWarpArrivalPosition(bonusMapInfo.arrival.cell, bonusMapInfo.arrival.exitType, BONUS_AREA_COLUMN_OFFSET), .exitType = bonusMapInfo.arrival.exitType };
    m_BonusAreaReturn = AreaArrival{ .position = WorldBuilder::GetWarpArrivalPosition(warp.returnPoint.cell, warp.returnPoint.exitType), .exitType = warp.returnPoint.exitType };
}

std::string superMarioBros::GameManager::GetCurrentLevelName()
{
    if (m_Levels.empty())
        LoadLevelData();

    const int levelIndex = m_PlayerInfo.currentLevel - 1;
    if (levelIndex < 0 || levelIndex >= static_cast<int>(m_Levels.size()))
        return "";

    return m_Levels[levelIndex].name;
}

void superMarioBros::GameManager::LoadLevelData()
{
    const JSONLoader::json data = JSONLoader::Load(LEVEL_DATA_PATH);

    m_TransitionMapName = data.at("transitionMap").get<std::string>();

    m_Levels = std::vector<LevelEntry>();
    for (const auto& levelData : data.at("levels"))
        m_Levels.push_back(LevelEntry{ .name = levelData.at("name").get<std::string>(), .map = levelData.at("map").get<std::string>() });

    m_Maps = std::unordered_map<std::string, MapInfo>();
    for (const auto& mapItem : data.at("maps").items())
        m_Maps[mapItem.key()] = ParseMapInfo(mapItem.value());
}

const superMarioBros::MapInfo& superMarioBros::GameManager::GetMapInfo(const std::string& mapName) const
{
    const auto it = m_Maps.find(mapName);
    if (it == m_Maps.end())
        throw std::runtime_error("Map not found in level data: " + mapName);

    return it->second;
}

const superMarioBros::LevelEntry& superMarioBros::GameManager::GetCurrentLevelEntry() const
{
    const int levelIndex = m_PlayerInfo.currentLevel - 1;
    if (levelIndex < 0 || levelIndex >= static_cast<int>(m_Levels.size()))
        throw std::runtime_error("Invalid Level.");

    return m_Levels[levelIndex];
}

void superMarioBros::GameManager::ReadLevelInfo(const std::string& filepath)
{
    ReadLevelFile(filepath, m_LevelInfo, m_Rows, m_Cols);
}

void superMarioBros::GameManager::ReadLevelFile(const std::string& filepath, std::vector<char>& levelInfo, int& rows, int& cols)
{
    std::ifstream file(filepath);
    if (file.is_open() == false)
        throw std::runtime_error("Could not open level file: " + filepath);

    std::string line;
    rows = 0;
    cols = 0;
    levelInfo = std::vector<char>();

    while (std::getline(file, line))
    {
        int colCount = 0;

        for (const char c : line)
        {
            // files saved with windows line endings (CRLF) leave a '\r' at the end of each line which is not a tile
            if (c == '\r')
                continue;

            levelInfo.push_back(c);
            ++colCount;
        }

        if (rows == 0)
            cols = colCount;

        ++rows;
    }


    file.close();
}

void superMarioBros::GameManager::ClearListeners()
{
    OnNewLevelLoadedEvent.ClearAllListeners();
    OnPlayerSwitchedEvent.ClearAllListeners();
    OnScoreAddedEvent.ClearAllListeners();
    OnCoinCollectedEvent.ClearAllListeners();
    OnLevelClearedEvent.ClearAllListeners();
    OnBonusAreaTriggeredEvent.ClearAllListeners();
}
