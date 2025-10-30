#pragma once
#include <memory>
#include <string>
#include <SFML/System/Vector2.hpp>

#include "Engine/Interfaces/EventSystem.h"
#include "Engine/Singleton/Singleton.h"
#include "Engine/Collision/Collider.h"

namespace thomasWasLate
{
    enum class PlayerHealthState : std::uint8_t
    {
        Small = 0,
        Big = 1,
        Fire = 2
    };
    
    class GameManager final : public diji::Singleton<GameManager>
    {
    public:
        [[nodiscard]] PlayerHealthState GetCurrentPlayerState() const { return m_CurrentPlayerState; }

        diji::Event<> OnPlayerSwitchedEvent;
        diji::Event<> OnNewLevelLoadedEvent;
        diji::Event<int> OnScoreAddedEvent;
        diji::Event<> OnCoinCollectedEvent;
        diji::Event<> OnLevelClearedEvent;

        void LoadLevel();
        void ClearLevelInfo() { m_LevelInfo = std::vector<char>(); }
        void SwitchToNextScene();
        [[nodiscard]] const std::vector<char>& GetLevelInfo() const { return m_LevelInfo; }
        [[nodiscard]] const sf::Vector2u& GetStartPosition() const { return m_StartPosition; }
        [[nodiscard]] int GetRows() const { return m_Rows; }
        [[nodiscard]] int GetCols() const { return m_Cols; }
        void SetLevelCleared();
        void ResetLevel();
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
        };
        [[nodiscard]] const PlayerInfo& GetPlayerInfo() const { return m_PlayerInfo; }
        void LoseLife() { --m_PlayerInfo.totalLives; }
        void AddLife() { ++m_PlayerInfo.totalLives; }
        void SaveScore(const int score) { m_PlayerInfo.totalScore = score; }
        void SaveCoins(const int coins) { m_PlayerInfo.totalCoins = coins; }
        void ResetPlayerInfo();
        [[nodiscard]] std::unordered_set<const diji::Collider*> GetEnemyColliders() const { return m_EnemyColliders; }
        
    private:
        std::unordered_set<const diji::Collider*> m_EnemyColliders;
        std::vector<std::unique_ptr<diji::Collider>> m_TileColliders;
        PlayerHealthState m_CurrentPlayerState = PlayerHealthState::Small;
        sf::Vector2u m_StartPosition;
        std::vector<char> m_LevelInfo;
        int m_Rows = 0;
        int m_Cols = 0;
        int m_TotalFireballsInLevel = 0;
        bool m_ShouldPlayTransition = false;
        PlayerInfo m_PlayerInfo;

        std::string LoadInformation();
        void ReadLevelInfo(const std::string& filepath);
        void CreateWorldCollision();
        void AddEnemyCollider(const diji::Collider* collider) { m_EnemyColliders.insert(collider); }
        void ClearListeners();
    };
}
