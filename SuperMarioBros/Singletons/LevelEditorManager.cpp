#include "LevelEditorManager.h"

#include "GameManager.h"
#include "../Helpers/WorldBuilder.h"
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

    GameManager::GetInstance().CreateEmptyWorld();
    WorldBuilder::Init();
}

void superMarioBros::LevelEditorManager::LoadLevel(const std::string& levelName)
{
    GameManager::GetInstance().LoadLevel(m_LevelNameToFileUMap[levelName]);
    m_CurrentLevelName = levelName;
}

void superMarioBros::LevelEditorManager::LoadNewLevel()
{
    GameManager::GetInstance().EmptyLevel();
    m_LevelInfo = std::vector<char>();
    m_CurrentLevelName = "New Level";
    m_LevelWidth = -1;
}

void superMarioBros::LevelEditorManager::SaveNewMap(const std::string& levelName, const std::string& levelPath)
{
    JSONLoader::json data;

    m_LevelNameToFileUMap[levelName] = levelPath;
    for (const auto& [key, value] : m_LevelNameToFileUMap) // todo: can I just add the new entry instead of rewriting the whole file?
    {
        data[key] = value;
    }

    JSONLoader::Save(LEVEL_EDITOR_INFO_PATH, data);

    SaveLevelInfo(MAPS_PATH + levelPath);
}

void superMarioBros::LevelEditorManager::SaveMap()
{
    SaveNewMap(m_CurrentLevelName, m_LevelNameToFileUMap.contains(m_CurrentLevelName) ? m_LevelNameToFileUMap[m_CurrentLevelName] : m_CurrentLevelName); // todo: add .txt to failstate
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

int superMarioBros::LevelEditorManager::SetCharAtPosition(const int x, const int y, const char value)
{
    if (x < 0)
        return -1;

    if (y < 0 || y >= MAX_LEVEL_HEIGHT)
        return -1;

    if (x >= m_LevelWidth)
    {
        const int newWidth = x + 1;

        std::vector newLevel(static_cast<size_t>(newWidth) * MAX_LEVEL_HEIGHT, '0');
        for (int row = 0; row < MAX_LEVEL_HEIGHT; ++row)
        {
            for (int col = 0; col < m_LevelWidth; ++col)
            {
                newLevel[row * newWidth + col] = m_LevelInfo[row * m_LevelWidth + col];
            }
        }

        m_LevelInfo = std::move(newLevel);
        m_LevelWidth = newWidth;
    }

    m_LevelInfo[y * m_LevelWidth + x] = value;

    return m_LevelWidth;
}

char superMarioBros::LevelEditorManager::GetLevelInfoAtPos(const int x, const int y) const
{
    if (x < 0 || y < 0 || y >= MAX_LEVEL_HEIGHT)
        return '0';

    const int cols = static_cast<int>(m_LevelInfo.size()) / MAX_LEVEL_HEIGHT;

    if (x >= cols)
        return '0';

    return m_LevelInfo[y * cols + x];
}

void superMarioBros::LevelEditorManager::SaveLevelInfo(const std::string& filepath) const
{
    if (m_LevelInfo.size() % MAX_LEVEL_HEIGHT != 0)
        throw std::runtime_error("Level data is not divisible by 13.");

    constexpr int rows = MAX_LEVEL_HEIGHT;
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

    file.close();
}

void superMarioBros::LevelEditorManager::RecalculateLevelWidth()
{
    if (m_LevelInfo.empty())
    {
        m_LevelWidth = 0;
        return;
    }

    const int rowWidth = static_cast<int>(m_LevelInfo.size()) / MAX_LEVEL_HEIGHT;

    int maxWidth = 0;

    for (int y = 0; y < MAX_LEVEL_HEIGHT; ++y)
    {
        const int rowStart = y * rowWidth;
        if (m_LevelInfo[rowStart + rowWidth] != '0')
        {
            m_LevelWidth = rowWidth;
            return;
        }

        int lastNonZero = -1;

        for (int x = 0; x < rowWidth; ++x)
        {
            if (m_LevelInfo[rowStart + x] != '0')
                lastNonZero = x;
        }

        if (lastNonZero != -1)
            maxWidth = std::max(maxWidth, lastNonZero + 1);
    }

    m_LevelWidth = maxWidth;
}
