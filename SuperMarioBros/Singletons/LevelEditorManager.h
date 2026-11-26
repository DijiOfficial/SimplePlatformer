#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <SFML/System/Vector2.hpp>

#include "../Helpers/LevelData.h"
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

        [[nodiscard]] const MapInfo& GetCurrentMapInfo() const { return m_CurrentMapInfo; }
        [[nodiscard]] const std::optional<WarpInfo>& GetPendingWarp() const { return m_PendingWarp; }
        [[nodiscard]] bool IsWaitingForWarpTarget() const { return m_PendingWarp.has_value() && m_PendingWarp->targetMap.empty(); }
        [[nodiscard]] bool IsWaitingForWarpReturn() const { return m_PendingWarp.has_value() && m_PendingWarp->targetMap.empty() == false; }
        [[nodiscard]] std::string GetMissingSaveRequirement() const;
        void SetStartCell(const sf::Vector2i& cell);
        void SetBonusAreaArrival(const WarpArrival& arrival);
        void SetBonusAreaExit(const WarpEntrance& exit);
        void BeginWarp(const WarpEntrance& entrance);
        void SetPendingWarpTarget(const std::string& targetMap);
        void CompleteWarp(const WarpArrival& returnPoint);
        void CancelPendingWarp();
        void SetBackground(const std::string& background);
        [[nodiscard]] bool TryRemoveMarkerAt(const sf::Vector2i& cell);
        [[nodiscard]] std::string CreateBonusMap();

        diji::Event<> OnLevelLoadedEvent;
        diji::Event<> OnMapInfoChangedEvent;
        diji::Event<> OnWarpTargetChosenEvent;

    private:
        std::string m_CurrentLevelName = "New Level";
        int m_LevelWidth = -1;
        std::unordered_map<std::string, std::string> m_LevelNameToFileUMap;
        std::vector<char> m_LevelInfo;
        MapInfo m_CurrentMapInfo;
        std::optional<WarpInfo> m_PendingWarp;

        void SaveLevelInfo(const std::string& filepath) const;
        void RecalculateLevelWidth();
    };
}
