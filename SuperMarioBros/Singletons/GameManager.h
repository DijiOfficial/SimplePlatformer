#pragma once
#include "Engine/Interfaces/EventSystem.h"
#include "Engine/Singleton/Singleton.h"
#include "Engine/Collision/Collider.h"
#include "../Helpers/LevelData.h"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
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

    struct AreaArrival
    {
        sf::Vector2f position;
        WarpExitType exitType = WarpExitType::Drop;
    };

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
        void DestroyWorld();
        void BuildEditorWorld(const std::vector<char>& levelInfo);
        void PrepareLevelTest(const sf::Vector2i& startCell);
        void EmptyLevel();
        void ClearLevelInfo() { m_LevelInfo = std::vector<char>(); }
        void SwitchToNextScene();
        [[nodiscard]] const std::vector<char>& GetLevelInfo() const { return m_LevelInfo; }
        [[nodiscard]] const sf::Vector2u& GetStartPosition() const { return m_StartPosition; }
        [[nodiscard]] std::string GetCurrentLevelName();
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
        static void ApplyBackground(const std::string& background);

        struct PlayerInfo
        {
            int totalLives = 3;
            int totalCoins = 0;
            int totalScore = 0;
            int currentLevel = 1;
            bool checkPointActivated = false;
            sf::Vector2f checkPointPosition = { 0.0f, 0.0f };
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
        void HideCurrentArea() const;
        [[nodiscard]] AreaArrival ShowNextArea();
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

        // Level Data
        std::string m_TransitionMapName;
        std::vector<LevelEntry> m_Levels;
        std::unordered_map<std::string, MapInfo> m_Maps;

        // Bonus Area
        diji::GameObject* m_BonusAreaWorldGameObject = nullptr;
        std::vector<char> m_BonusAreaInfo;
        AreaArrival m_BonusAreaArrival;
        AreaArrival m_BonusAreaReturn;
        std::string m_LevelBackground;
        std::string m_BonusAreaBackground;
        int m_BonusAreaRows = 0;
        int m_BonusAreaCols = 0;
        bool m_IsInBonusArea = false;

        void LoadLevelData();
        [[nodiscard]] const MapInfo& GetMapInfo(const std::string& mapName) const;
        [[nodiscard]] const LevelEntry& GetCurrentLevelEntry() const;
        void ReadLevelInfo(const std::string& filepath);
        void CreateWorld();
        static void ReadLevelFile(const std::string& filepath, std::vector<char>& levelInfo, int& rows, int& cols);
        void ResetBonusArea();
        void LoadBonusArea(const WarpInfo& warp);
        void ClearListeners();
        void SavePlayerState();
    };
}
