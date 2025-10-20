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
        void LoadLevel();
        void ClearLevelInfo() { m_LevelInfo = std::vector<char>(); }
        [[nodiscard]] const std::vector<char>& GetLevelInfo() const { return m_LevelInfo; }
        [[nodiscard]] const sf::Vector2u& GetStartPosition() const { return m_StartPosition; }
        // [[nodiscard]] int GetCurrentLevel() const { return m_CurrentLevel; }
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
    private:
        std::vector<std::unique_ptr<diji::Collider>> m_TileColliders;
        PlayerHealthState m_CurrentPlayerState = PlayerHealthState::Small;
        sf::Vector2u m_StartPosition;
        std::vector<char> m_LevelInfo;
        int m_CurrentLevel = 1;
        int m_Rows = 0;
        int m_Cols = 0;
        int m_TotalFireballsInLevel = 0;

        std::string LoadInformation();
        void ReadLevelInfo(const std::string& filepath);
        void CreateWorldCollision() const;
    };
}
