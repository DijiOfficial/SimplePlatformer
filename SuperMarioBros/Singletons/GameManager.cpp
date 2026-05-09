#include "GameManager.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "../Core/GameState.h"
#include "../Components/Other/HUD/PointsBehaviour.h"
#include "../Components/Player/PlayerCharacter.h"

#include <format>
#include <fstream>

#include "../Helpers/WorldBuilder.h"

namespace superMarioBros
{
    class GoombaAI;
    class PointsBehaviour;
}

void superMarioBros::GameManager::LoadLevel()
{
    m_IsLevelAlreadyCleared = false;
    m_CurrentPlayerState = m_LastPlayerState == PlayerHealthState::Small ? PlayerHealthState::Small : PlayerHealthState::Big;
    ReadLevelInfo(LoadInformation());

    WorldBuilder::CreateWorld(m_LevelInfo, m_Rows, m_Cols);

    OnNewLevelLoadedEvent.Broadcast();
}

void superMarioBros::GameManager::LoadLevel(const std::string& levelFilePath)
{
    ReadLevelInfo(std::format("../SuperMarioBros/Resources/levels/{}", levelFilePath));
    WorldBuilder::CreateWorld(m_LevelInfo, m_Rows, m_Cols);
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

    if (m_PlayerInfo.currentLevel >= 4)
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
    if (!file.is_open())
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
    if (!file.is_open())
        throw std::runtime_error("Could not open high score file: " + m_HighScoreFileName);


    file << m_PlayerInfo.totalScore;
    file.close();
}

std::string superMarioBros::GameManager::LoadInformation()
{
    if (m_ShouldPlayTransition)
    {
        m_ShouldPlayTransition = false;
        m_StartPosition.x = 100;
        m_StartPosition.y = 100;

        return MAPS_PATH + "transitionLevel.txt"; //todo: save file name
    }
    
    switch (m_PlayerInfo.currentLevel) // if you're going to read from a file put this information in the fucking file
    {
    case 1:
        m_StartPosition.x = 100;
        m_StartPosition.y = 400;
        break;
    case 2:
        m_StartPosition.x = 100;
        m_StartPosition.y = 0;
        break;
    case 3:
        m_StartPosition.x = 1250;
        m_StartPosition.y = 0;
        break;
    case 4:
        m_StartPosition.x = 50;
        m_StartPosition.y = 200;
        break;
    default:
        throw std::runtime_error("Invalid Level.");
    }

    return std::format("{}level{}.txt", MAPS_PATH, m_PlayerInfo.currentLevel);
}

void superMarioBros::GameManager::ReadLevelInfo(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Could not open level file: " + filepath);

    std::string line;
    m_Rows = 0;
    m_Cols = 0;
    m_LevelInfo = std::vector<char>();

    while (std::getline(file, line))
    {
        int colCount = 0;

        for (const char c : line)
        {
            m_LevelInfo.push_back(c);
            ++colCount;
        }

        if (m_Rows == 0)
            m_Cols = colCount;

        ++m_Rows;
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
}
