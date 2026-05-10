#pragma once
#include <string>
#include <unordered_map>
#include <SFML/System/Vector2.hpp>

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
        void SaveMap(const std::string& levelName) ;
        std::vector<std::string> GetLevelNames() const;
        sf::Vector2f m_SelectorPosition;

    private:
        const std::string LEVEL_EDITOR_INFO_PATH = "../SuperMarioBros/Resources/levels/levelEditorInfo.json";
        const int MAX_LEVEL_HEIGHT = 12;
        std::unordered_map<std::string, std::string> m_LevelNameToFileUMap;
        std::vector<char> m_LevelInfo;

        void SaveLevelInfo(const std::string& filepath) const;
    };
}
