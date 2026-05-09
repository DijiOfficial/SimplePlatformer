#include "LevelEditorManager.h"

#include "GameManager.h"
#include "Engine/Singleton/JSONLoader.h"

void superMarioBros::LevelEditorManager::Init()
{
    const JSONLoader::json data = JSONLoader::Load(LEVEL_EDITOR_INFO_PATH);
    
    if (!data.is_object())
        throw std::runtime_error("LEVEL_EDITOR_INFO_PATH JSON root must be an object.");

    m_LevelNameToFileUMap.clear();

    for (auto it = data.begin(); it != data.end(); ++it)
    {
        if (it.value().is_string())
            m_LevelNameToFileUMap[it.key()] = it.value().get<std::string>();
        else
            m_LevelNameToFileUMap[it.key()] = it.value().dump();
    }
}

void superMarioBros::LevelEditorManager::LoadLevel(const std::string& levelName)
{
    GameManager::GetInstance().LoadLevel(m_LevelNameToFileUMap[levelName]);
    m_LevelInfo = std::vector(GameManager::GetInstance().GetLevelInfo());
}

void superMarioBros::LevelEditorManager::SaveNewMap(const std::string& levelName, const std::string& levelPath)
{
    JSONLoader::json data;

    m_LevelNameToFileUMap[levelName] = levelPath;
    for (const auto& [key, value] : m_LevelNameToFileUMap)
    {
        data[key] = value;
    }

    JSONLoader::Save(LEVEL_EDITOR_INFO_PATH, data);

    SaveLevelInfo(MAPS_PATH + levelPath);
}

void superMarioBros::LevelEditorManager::SaveMap(const std::string& levelName)
{
    SaveNewMap(levelName, m_LevelNameToFileUMap.contains(levelName) ? m_LevelNameToFileUMap[levelName] : levelName);
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

void superMarioBros::LevelEditorManager::SaveLevelInfo(const std::string& filepath) const
{
    if (m_LevelInfo.size() % 12 != 0)
        throw std::runtime_error("Level data is not divisible by 12.");
    
    const int rows = MAX_LEVEL_HEIGHT;
    const int cols = static_cast<int>(m_LevelInfo.size()) / rows;
    
    std::ofstream file(filepath);

    if (!file.is_open())
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

    for (int col = 0; col < cols; ++col)
        file << '0';

    file.close();
}
