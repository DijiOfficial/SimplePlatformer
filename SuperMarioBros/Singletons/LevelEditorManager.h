#pragma once
#include <string>
#include <unordered_map>
#include <SFML/System/Vector2.hpp>

#include "Engine/Interfaces/EventSystem.h"
#include "Engine/Singleton/Singleton.h"

namespace superMarioBros
{

    class LevelEditorManager final : public diji::Singleton<LevelEditorManager>
    {
    public:
        void Init();
        void LoadLevel(const std::string& levelName);
        void LoadNewLevel();
        void SaveNewMap(const std::string& levelName, const std::string& levelPath);
        void SaveMap() ;
        [[nodiscard]] std::vector<std::string> GetLevelNames() const;
        [[nodiscard]] sf::Vector2f m_SelectorPosition;

        [[nodiscard]] const std::string& GetCurrentLevelName() const { return m_CurrentLevelName; }
        [[nodiscard]] bool DoesLevelExist(const std::string& levelName) const { return m_LevelNameToFileUMap.contains(levelName); }
        void SetLevelInfo(const std::vector<char>& levelInfo) { m_LevelInfo = levelInfo; RecalculateLevelWidth(); OnLevelLoadedEvent.Broadcast(); }
        [[nodiscard]] int SetCharAtPosition(int x, int y, char value);
        [[nodiscard]] const std::vector<char>& GetLevelInfo() const { return m_LevelInfo; }
        [[nodiscard]] char GetLevelInfoAtPos(int x, int y) const;
        [[nodiscard]] int GetLevelWidth() const { return m_LevelWidth; }

        diji::Event<> OnLevelLoadedEvent;

    private:
        const std::string LEVEL_EDITOR_INFO_PATH = "../SuperMarioBros/Resources/levels/levelEditorInfo.json";
        std::string m_CurrentLevelName = "New Level";
        int m_LevelWidth = -1;
        std::unordered_map<std::string, std::string> m_LevelNameToFileUMap;
        std::vector<char> m_LevelInfo;

        void SaveLevelInfo(const std::string& filepath) const;
        void RecalculateLevelWidth();
    };
}
