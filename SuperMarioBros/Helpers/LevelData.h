#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <SFML/System/Vector2.hpp>

namespace diji
{
    enum class Directions;
}

namespace superMarioBros
{
    static const std::string MAPS_PATH = "../SuperMarioBros/Resources/levels/";
    static const std::string LEVEL_DATA_PATH = MAPS_PATH + "levelData.json";

    enum class WarpExitType : std::uint8_t
    {
        Drop = 0,
        RiseFromPipe = 1
    };

    struct WarpEntrance
    {
        sf::Vector2i cell;
        diji::Directions direction{};
    };

    struct WarpArrival
    {
        sf::Vector2i cell;
        WarpExitType exitType = WarpExitType::Drop;
    };

    struct WarpInfo
    {
        WarpEntrance entrance;
        std::string targetMap;
        WarpArrival returnPoint;
    };

    struct MapInfo
    {
        std::string file;
        std::optional<sf::Vector2i> start;
        bool isBonusArea = false;
        std::optional<WarpArrival> arrival;
        std::optional<WarpEntrance> exit;
        std::vector<WarpInfo> warps;
    };

    struct LevelEntry
    {
        std::string name;
        std::string map;
    };

    struct LevelDataFile
    {
        std::string transitionMap;
        std::vector<LevelEntry> levels;
        std::unordered_map<std::string, MapInfo> maps;
    };

    class LevelData final
    {
    public:
        [[nodiscard]] static LevelDataFile Load();
        static void SaveMapInfo(const std::string& mapName, const MapInfo& mapInfo);
    };
}
