#pragma once
#include <memory>
#include <string>
#include <SFML/System/Vector2.hpp>

#include "Engine/Interfaces/EventSystem.h"
#include "Engine/Singleton/Singleton.h"
#include "Engine/Collision/Collider.h"

namespace thomasWasLate
{
    enum class CurrentPlayer
    {
        Thomas = 0,
        Bob
    };
    
    class GameManager final : public diji::Singleton<GameManager>
    {
    public:
        [[nodiscard]] CurrentPlayer GetCurrentPlayer() const { return m_CurrentPlayer; }
        void SwitchPlayer();

        diji::Event<> OnPlayerSwitchedEvent;
        diji::Event<> OnNewLevelLoadedEvent;
        void LoadLevel();
        void ClearLevelInfo() { m_LevelInfo = std::vector<int>(); }
        [[nodiscard]] const std::vector<int>& GetLevelInfo() const { return m_LevelInfo; }
        [[nodiscard]] const sf::Vector2u& GetStartPosition() const { return m_StartPosition; }
        // [[nodiscard]] int GetCurrentLevel() const { return m_CurrentLevel; }
        [[nodiscard]] int GetRows() const { return m_Rows; }
        [[nodiscard]] int GetCols() const { return m_Cols; }
        void SetLevelCleared();
    
    private:
        std::vector<std::unique_ptr<diji::Collider>> m_TileColliders;
        CurrentPlayer m_CurrentPlayer = CurrentPlayer::Thomas;
        sf::Vector2u m_StartPosition;
        std::vector<int> m_LevelInfo;
        int m_CurrentLevel = 1;
        int m_Rows = 0;
        int m_Cols = 0;
        float m_LevelTimeLimit = 0.0f;

        std::string LoadInformation();
        void ReadLevelInfo(const std::string& filepath);
        void CreateWorldCollision();
    };
}
