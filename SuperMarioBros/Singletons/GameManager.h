#pragma once
#include "Engine/Interfaces/EventSystem.h"
#include "Engine/Singleton/Singleton.h"
#include "Engine/Collision/Collider.h"

#include <memory>
#include <string>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace diji
{
    enum class Directions;
}

namespace superMarioBros
{
    enum class PlayerHealthState : std::uint8_t
    {
        Small = 0,
        Big = 1,
        Fire = 2
    };

    static const std::string MAPS_PATH = "../SuperMarioBros/Resources/levels/";
    
    class GameManager final : public diji::Singleton<GameManager>
    {
    public:
        [[nodiscard]] PlayerHealthState GetCurrentPlayerState() const { return m_CurrentPlayerState; }
        [[nodiscard]] PlayerHealthState GetLastPlayerState() const { return m_LastPlayerState; }

        diji::Event<> OnPlayerSwitchedEvent;
        diji::Event<> OnNewLevelLoadedEvent;
        diji::Event<int> OnScoreAddedEvent;
        diji::Event<> OnCoinCollectedEvent;
        diji::Event<> OnLevelClearedEvent;
        diji::Event<diji::Directions> OnBonusAreaTriggeredEvent;

        void LoadLevel();
        void LoadLevel(const std::string& levelFilePath);
        void EmptyLevel();
        void ClearLevelInfo() { m_LevelInfo = std::vector<char>(); }
        void SwitchToNextScene();
        [[nodiscard]] const std::vector<char>& GetLevelInfo() const { return m_LevelInfo; }
        [[nodiscard]] const sf::Vector2u& GetStartPosition() const { return m_StartPosition; }
        [[nodiscard]] int GetRows() const { return m_Rows; }
        [[nodiscard]] int GetCols() const { return m_Cols; }
        void SetLevelCleared();
        void ResetLevel(bool playerDied = true);
        void SwitchCurrentPlayerState();
        void SetPlayerState(const PlayerHealthState newState) { m_CurrentPlayerState = newState; }
        void FireballAdded() { ++m_TotalFireballsInLevel; }
        void FireballRemoved() { --m_TotalFireballsInLevel; }
        [[nodiscard]] bool CanSpawnFireball() const { return m_TotalFireballsInLevel < 2; }

        static void SpawnPointsText(const sf::Vector2f& position, const std::string& score);

        struct PlayerInfo
        {
            int totalLives = 3;
            int totalCoins = 0;
            int totalScore = 0;
            int currentLevel = 1;
            bool checkPointActivated = false;
            sf::Vector2f checkPointPosition = { 0.f, 0.f };
        };
        [[nodiscard]] const PlayerInfo& GetPlayerInfo() const { return m_PlayerInfo; }
        void LoseLife() { --m_PlayerInfo.totalLives; }
        void AddLife();
        void TriggerCheckPoint(const sf::Vector2f& pos) { m_PlayerInfo.checkPointActivated = true; m_PlayerInfo.checkPointPosition = pos; }
        void SaveScore(const int score) { m_PlayerInfo.totalScore = score; }
        void SaveCoins(const int coins) { m_PlayerInfo.totalCoins = coins; }
        void ResetPlayerInfo();
        [[nodiscard]] std::unordered_set<diji::Collider*> GetEnemyColliders() const { return m_EnemyColliders; }
        [[nodiscard]] bool IsCheckPointActivated() const { return m_PlayerInfo.checkPointActivated; }
        [[nodiscard]] const sf::Vector2f& GetCheckPointPosition() const { return m_PlayerInfo.checkPointPosition; }

        [[nodiscard]] int GetHighScoreFromFile() const;
        void SaveHighScoreToFile() const;
        void AddEnemyCollider(diji::Collider* collider) { m_EnemyColliders.insert(collider); }
        void CreateEmptyWorld();
        const diji::GameObject* PlaceNewItem(int x, int y, char itemID) const;
        void AttachToWorldObject(const diji::GameObject* object) const;

        static constexpr int BONUS_AREA_COLUMN_OFFSET = -80;

        void PrepareLoadBonusArea(diji::Directions direction);
        void HideCurrentArea();
        [[nodiscard]] sf::Vector2f ShowNextArea();
        void SetBonusAreaExitPosition(const sf::Vector2f& position) { m_BonusAreaExitPosition = position; }
        [[nodiscard]] bool IsInBonusArea() const { return m_IsInBonusArea; }
        [[nodiscard]] const std::vector<char>& GetBonusAreaInfo() const { return m_BonusAreaInfo; }
        [[nodiscard]] int GetBonusAreaRows() const { return m_BonusAreaRows; }
        [[nodiscard]] int GetBonusAreaCols() const { return m_BonusAreaCols; }

    private:
        diji::GameObject* m_WorldGameObject = nullptr;
        std::unordered_set<diji::Collider*> m_EnemyColliders;
        std::vector<std::unique_ptr<diji::Collider>> m_TileColliders;
        PlayerHealthState m_CurrentPlayerState = PlayerHealthState::Small;
        PlayerHealthState m_LastPlayerState = PlayerHealthState::Small;
        sf::Vector2u m_StartPosition;
        std::vector<char> m_LevelInfo;
		// todo: remove string literal
        std::string m_HighScoreFileName = "../SuperMarioBros/Resources/levels/highscores.txt";
        int m_Rows = 0;
        int m_Cols = 0;
        int m_TotalFireballsInLevel = 0;
        bool m_ShouldPlayTransition = false;
        bool m_IsLevelAlreadyCleared = false;
        PlayerInfo m_PlayerInfo;

        // Bonus Area
        diji::GameObject* m_BonusAreaWorldGameObject = nullptr;
        std::vector<char> m_BonusAreaInfo;
        sf::Vector2f m_BonusAreaSpawnPosition;
        sf::Vector2f m_BonusAreaExitPosition;
        sf::Color m_LevelBackgroundColor;
        int m_BonusAreaRows = 0;
        int m_BonusAreaCols = 0;
        bool m_IsInBonusArea = false;

        std::string LoadInformation();
        void ReadLevelInfo(const std::string& filepath);
        static void ReadLevelFile(const std::string& filepath, std::vector<char>& levelInfo, int& rows, int& cols);
        void LoadBonusArea();
        std::string LoadBonusAreaInformation();
        void ClearListeners();
        void SavePlayerState();
    };
}
